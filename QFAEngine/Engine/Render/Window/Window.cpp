#include "Window.h"

#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>

#include <Render/Time.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Overlord/Overlord.h>
#include <Render/Window/Viewport.h>
#include <chrono>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Render/UI/UIImage.h>

#include <Render/UI/Text.h>
#include <Render/RenderPass/RenderPassSwapChain.h>

#include <Render/Framebuffer/MeshFrameBuffer.h> 
#include <Render/UI/UIParentMultipleUnit.h>
#include <Render/Pipline/Pipline.h>
#include <Render/UI/UIParentOneUnit.h>
#include <Render/UI/UIParentHiddenChild.h>

VkCommandPool QFAWindow::commandPool;

QFAVKShadowFrameBuffer* QFAWindow::ShadowFrameBuffer;
QFAVKImageView* QFAWindow::ShadowImagesView;
QFAVKTextureSampler* QFAWindow::ShadowSampler;

std::vector<QFAWindow::SViewportStuff> QFAWindow::ViewportStuff;
int QFAWindow::ViewportProcess;
VkSemaphore* QFAWindow::NextWaitSemi;
QFAWindow* QFAWindow::CurentProcessWindow;


QFAVKRenderPassSwapChain* QFAWindow::RenderPassSwapChain;
QFAVKRenderPass* QFAWindow::RenderPass;
QFAVKRenderPassDepth* QFAWindow::RenderPassOffScreen;
QFAVKTextRenderPass* QFAWindow::TextRenderPass;


VkFormat QFAWindow::depthFormat =  VK_FORMAT_D32_SFLOAT;
std::vector<QFAWindow*> QFAWindow::Windows;

QFAVKInstance* QFAWindow::Instance = nullptr;
QFAImage* QFAWindow::ShadowImage = nullptr;

VkCommandBuffer QFAWindow::FinisCommandBuffer;
/*-----*/


QFAWindow::QFAWindow(int width, int height, std::string name)
{
	Windows.push_back(this);
	Width = width;
	Height = height;

	QFAWindow::CurentProcessWindow = this;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	QFAInput::WindowCreated(this);
	UIEvent = new QFAUIEvent(this, glfWindow);
	glfwSetWindowUserPointer(glfWindow, this);
	//glfwSetFramebufferSizeCallback(glfWindow, framebufferResizeCallback);

	if (!glfWindow)
	{
		glfwTerminate();
		ASSERT(false);
		return;
	}

	glfwSetWindowSizeCallback(glfWindow, [](GLFWwindow* win, int w, int h)
	{		
		for (size_t i = 0; i < QFAWindow::Windows.size(); i++)
		{
			if (QFAWindow::Windows[i]->glfWindow == win)
			{		
				if (w == 0 || h == 0)
				{
					QFAWindow::Windows[i]->minimized = true;
					return;
				}
				else
					QFAWindow::Windows[i]->minimized = false;

				QFAWindow::Windows[i]->NewWidth = w;
				QFAWindow::Windows[i]->NewHeight = h;
				QFAWindow::Windows[i]->WindowSizeChanched = true;
				return;
			}
		}
	});

	glfwSetCursorEnterCallback(glfWindow, [](GLFWwindow* window, int entered)
	{
		for (size_t i = 0; i < QFAWindow::Windows.size(); i++)
		{
			if (QFAWindow::Windows[i]->glfWindow == window)
			{
				QFAWindow::Windows[i]->CursorOnWindow = entered;
				return;
			}
		}			
	});	

	if(!Instance)
		Instance = new QFAVKInstance;

	if (glfwCreateWindowSurface(Instance->instance, glfWindow, nullptr, &surface) != VK_SUCCESS)
		stopExecute("QFAWindow::QFAWindow glfwCreateWindowSurface error");

	if (Windows.size() == 1)
	{
		QFAVKPhysicalDevice::Init(Instance, surface);
		QFAVKLogicalDevice::Init(surface);
		QFAVKBuffer::Init(Instance->instance);
		createCommandPool();
	}
	
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);

	if (Windows.size() == 1)
	{
		RenderPassOffScreen = new QFAVKRenderPassDepth();
		RenderPass = new QFAVKRenderPass(SwapChain->swapChainImageFormat, true);
		TextRenderPass = new QFAVKTextRenderPass(SwapChain->swapChainImageFormat);
		RenderPassSwapChain = new QFAVKRenderPassSwapChain(SwapChain->swapChainImageFormat, true);
	
		QFAText::Init(TextRenderPass->renderPass, commandPool);
		QFAUIImage::Init(TextRenderPass->renderPass, commandPool);

		ShadowFrameBuffer = new QFAVKShadowFrameBuffer(commandPool, RenderPassOffScreen->renderPass);
		CreateShadow();

		QMeshBaseComponent::Init(RenderPass->renderPass, RenderPassOffScreen->renderPass, commandPool);

		createCommandBuffer();
	}
	


	SwapChain->createFramebuffers(RenderPassSwapChain->renderPass);	
	frameBuffer = new QFAVKMeshFrameBuffer(commandPool, width, height);
	PresentImage = new QFAPresentImage(commandPool, RenderPassSwapChain->renderPass, frameBuffer->ColorImage);

	

	createSyncObject();
	
	QFAViewport* vp = new QFAViewport;	
	AddViewport(vp);
	vp->Settup(width, height);


}

QFAWindow::~QFAWindow()
{
	for (size_t i = 0; i < Viewports.Length(); i++)
		Viewports[i]->WindowRemoveMe();

	delete UIEvent;
	QFAInput::WindowClosed(this);
	glfwDestroyWindow(glfWindow);
}

void QFAWindow::createCommandBuffer()
{
	std::array<VkCommandBuffer,  1> comb;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)comb.size();

	if (vkAllocateCommandBuffers(QFAVKLogicalDevice::GetDevice(), &allocInfo, comb.data()) != VK_SUCCESS)
		stopExecute("failed to allocate command buffers!");

	FinisCommandBuffer = comb[comb.size() - 1];	
}

void QFAWindow::DrawUI(QFAViewport* viewport, int viewportIndex)
{
	CurentComandBuffer = ViewportStuff[ViewportProcess].comandBuffer;
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = TextRenderPass->renderPass;
	renderPassInfo.framebuffer = frameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->swapChainExtent;

	std::array<VkClearValue, 3> clearValues{};
	clearValues[0].color = { {0, 0, 0, 0} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(CurentComandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 
	/*---*/
	QFAVKPipeline* pipeline = nullptr;



	VkViewport vkViewport{};
	vkViewport.x = (float)viewport->X;
	vkViewport.y = (float)viewport->Y;
	vkViewport.width = (float)viewport->Width;
	vkViewport.height = (float)viewport->Height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;
	vkCmdSetViewport(CurentComandBuffer, 0, 1, &vkViewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapChain->swapChainExtent;
	vkCmdSetScissor(CurentComandBuffer, 0, 1, &scissor);

	StartUIRenderViewPort(viewportIndex);

	SortUIs(&viewport->Root); // parent should add before children
	for (size_t i = 0; i < SortUIUnits.Length(); i++)
	{
		if (SortUIUnits[i]->CanRender)
		{
			if (pipeline != ((QFAUIRenderUnit*)SortUIUnits[i])->GetPipeline())
			{
				pipeline = ((QFAUIRenderUnit*)SortUIUnits[i])->GetPipeline();
				vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
			}

			((QFAUIRenderUnit*)SortUIUnits[i])->Render(CurentComandBuffer);
		}
		else if (SortUIUnits[i]->CanBeParent)
		{
			if (pipeline != ((QFAUIParent*)SortUIUnits[i])->GetBackgroundPipeline())
			{
				pipeline = ((QFAUIParent*)SortUIUnits[i])->GetBackgroundPipeline();
				vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
			}

			((QFAUIParent*)SortUIUnits[i])->RenderBackground(CurentComandBuffer);

			if (((QFAUIParent*)SortUIUnits[i])->GetParentType() == QFAUIParent::HiddenChild)
			{
				if (pipeline != ((QFAParentHiddenChild*)SortUIUnits[i])->GetChildPipeline())
				{
					pipeline = ((QFAParentHiddenChild*)SortUIUnits[i])->GetChildPipeline();
					vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
				}

				((QFAParentHiddenChild*)SortUIUnits[i])->RenderChild(CurentComandBuffer);
			}
		}
	}
	
	vkCmdEndRenderPass(CurentComandBuffer);

}

void QFAWindow::SortUIs(QFAViewportRoot* root)
{		
	SortUIUnits.Clear();
	for (size_t i = 0; i < root->Children.Length(); i++)
		AddUnit(root->Children[i]);
	// don't replase int because in "i" can be minus value 
	for (int i = (int)SortUIUnits.Length() - 2; i > 0; i--)
	{
		for (int j = 0; j <= i; j++)
		{ // in start of array number bigger
			if (SortUIUnits[j + 1]->ZIndex > SortUIUnits[j]->ZIndex) // ZIndex forward == 1 
			{
				QFAUIUnit* pTem = SortUIUnits[j];
				SortUIUnits[j] = SortUIUnits[j + 1];
				SortUIUnits[j + 1] = pTem;
			}
		}
	}	
}

void QFAWindow::AddUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	if (!unit->IsEnable)
		return;

	SortUIUnits.Add(unit);
	if (unit->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)unit;
		if (parent->GetParentType() == QFAUIParent::EParentType::OneChild)
			AddUnit(((QFAUIParentOneUnit*)unit)->Child);
		else if (parent->GetParentType() == QFAUIParent::EParentType::OneChild)
			AddUnit(((QFAParentHiddenChild*)parent)->GetChild());
		else  if (parent->GetParentType() == QFAUIParent::EParentType::MultipleChild)
		{
			QFAUIParentMultipleUnit* parentMultiple = (QFAUIParentMultipleUnit*)unit;
			for (size_t i = 0; i < parentMultiple->Children.Length(); i++)
				AddUnit(parentMultiple->Children[i]);
		}
	}	
}


void QFAWindow::StartUIRenderViewPort( int viewportIndex )
{	
	QFAText::CurentDescriptorSetProject = QFAText::Pipeline->GetSet(0, QFAWindow::ViewportProcess);
	QFAUIImage::CurentDescriptorSetProject = QFAUIImage::Pipeline->GetSet(0, QFAWindow::ViewportProcess);
	
	UniformBufferObject ubo{};
	ubo.projection = Viewports[viewportIndex]->UIProjection;
	memcpy(ViewportStuff[QFAWindow::ViewportProcess].buffers.uiProjectionBuffer->MapData, &ubo, sizeof(ubo.projection));
}

void QFAWindow::DrawOffscreenBuffer()
{

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPassSwapChain->renderPass;
	renderPassInfo.framebuffer = SwapChain->swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(FinisCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);// 


	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)Width;
	viewport.height = (float)Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(FinisCommandBuffer, 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapChain->swapChainExtent;
	vkCmdSetScissor(FinisCommandBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PresentImage->Pipeline->GetPipeline());

	recordCommandBufferTestImege();

	vkCmdEndRenderPass(FinisCommandBuffer);
}

void QFAWindow::recordCommandBufferTestImege()
{
	VkBuffer vertexBuffers[] = { PresentImage->vertexBufer->GpuSideBuffer->Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(FinisCommandBuffer, 0, 1, vertexBuffers, offsets);

	VkDescriptorSet descSet = PresentImage->Pipeline->GetSet(0, 0);

	vkCmdBindDescriptorSets(FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		PresentImage->Pipeline->GetPipelineLayout(), 0, 1, &descSet , 0, nullptr);
	
	vkCmdDraw(FinisCommandBuffer, static_cast<uint32_t>(6), 1, 0, 0);
}

void QFAWindow::QueueSubmitPresent(std::vector<VkSemaphore>& listSemi)
{

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = NextWaitSemi ? 1 : 0;
	submitInfo.pWaitSemaphores = NextWaitSemi;


	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &FinisCommandBuffer;
	
	submitInfo.signalSemaphoreCount = listSemi.size();
	submitInfo.pSignalSemaphores = listSemi.data();



	if (vkEndCommandBuffer(FinisCommandBuffer) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");

}

void QFAWindow::createSyncObject()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;	
	
	if(vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &GetImageSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemaphore) != VK_SUCCESS || 
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemiOtherWindow) != VK_SUCCESS)
		stopExecute("failed to create Semaphore");
}

void QFAWindow::recreateSwapChain()
{
	delete SwapChain;
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	SwapChain->createFramebuffers(RenderPass->renderPass);
	
	frameBuffer->ResizeBuffer(commandPool, NewWidth, NewHeight);
	delete PresentImage;
	PresentImage = new QFAPresentImage(commandPool, RenderPassSwapChain->renderPass, frameBuffer->ColorImage);	
}

void QFAWindow::AddViewport(QFAViewport* viewport)
{
	Viewports.Add(viewport);
	viewport->Settup(Width, Height);
	viewport->WindowAddMe(this);

	int activeViewportCount = 0;
	for (size_t i = 0; i < Windows.size(); i++)
		activeViewportCount += Windows[i]->Viewports.Length();

	if (ViewportStuff.size() < activeViewportCount)
		CreateViewPortStuff();
}

void QFAWindow::RemoveViewport(QFAViewport* viewport)
{
	if (Viewports.Remove(viewport))
		viewport->WindowRemoveMe();
}

QFAViewport* QFAWindow::GetViewport(size_t index)
{
	if (index < 0 || index >= Viewports.Length())
		return nullptr;
	else
		return Viewports[index];		
}

bool QFAWindow::GetMousePosition(double& x, double& y)
{
	if (CursorOnWindow)
	{
		glfwGetCursorPos(glfWindow, &x, &y);
		return true;
	}
	else
		return false;
}

void QFAWindow::createCommandPool()
{ 
	QFAVKStruct::QueueFamilyIndices queueFamilyIndices = QFAVKQueueFamilies::findQueueFamilies(QFAVKPhysicalDevice::GetDevice(), surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	
	if (vkCreateCommandPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		stopExecute("failed to create graphics command pool!");
}

void QFAWindow::BeginCommandBuffer()
{
	vkResetCommandBuffer(ViewportStuff[ViewportProcess].comandBuffer, 0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(ViewportStuff[ViewportProcess].comandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");
}

void QFAWindow::EndCommandBufferAndQueueSubmit()
{

	if (vkEndCommandBuffer(CurentComandBuffer) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = NextWaitSemi ? 1 : 0;
	submitInfo.pWaitSemaphores = NextWaitSemi;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CurentComandBuffer;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &ViewportStuff[ViewportProcess].semaphore.ActorShadow;
	NextWaitSemi = &ViewportStuff[ViewportProcess].semaphore.ActorShadow;


	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");
}

void QFAWindow::ShadowRender(QFAViewport* _viewport)
{
	CurentComandBuffer = ViewportStuff[ViewportProcess].comandBuffer;

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = RenderPassOffScreen->renderPass;
	renderPassInfo.framebuffer = ShadowFrameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { (uint32_t)shadowResolution, (uint32_t)shadowResolution };

	std::array<VkClearValue, 1> clearValues{};
	clearValues[0].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(CurentComandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 

	// Set depth bias (aka "Polygon offset")
// Required to avoid shadow mapping artifacts
	vkCmdSetDepthBias(
		CurentComandBuffer,
		depthBiasConstant,
		0.0f,
		depthBiasSlope);

	vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::ShadowPipline->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = shadowResolution;
	viewport.height = shadowResolution;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(CurentComandBuffer, 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { (uint32_t)shadowResolution, (uint32_t)shadowResolution };
	vkCmdSetScissor(CurentComandBuffer, 0, 1, &scissor);

	QWorld* world = _viewport->GetWorld();
	QDirectionLight* dl = world->GetDirectionDight();
	glm::mat4 liteMat = dl->GetLightMatrix();
	
	QMeshBaseComponent::ShaderDL.ambient = dl->Ambient;
	QMeshBaseComponent::ShaderDL.diffuse = dl->Diffuse;
	QMeshBaseComponent::ShaderDL.direction = dl->Direction;
	QMeshBaseComponent::ShaderDL.specular = dl->Specular;
	
	CurentCameraPosition = _viewport->CurentCamera->WorldPosition;
	QMeshBaseComponent::StartShadowFrameViewport(liteMat);
		
	unsigned int countComponentForRender = 0;
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, true);
	

	vkCmdEndRenderPass(CurentComandBuffer);

	VkImageSubresourceLayers isl;
	isl.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	isl.mipLevel = 0;
	isl.baseArrayLayer = 0;
	isl.layerCount = 1;

	VkImageCopy ic;
	ic.srcSubresource = isl;
	ic.dstSubresource = isl;
	ic.srcOffset = { 0,0 };
	ic.dstOffset = { 0,0 };
	ic.extent = { (uint32_t)shadowResolution, (uint32_t)shadowResolution, 1 };

	vkCmdCopyImage(CurentComandBuffer, ShadowFrameBuffer->depthImageQFA->TextureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ShadowImage->TextureImage, VK_IMAGE_LAYOUT_GENERAL, 1, &ic);

}


void QFAWindow::RenderWindows()
{ 
	ViewportProcess = 0;
	QMeshBaseComponent::StartFrame();
	QFAText::StartFrame();
	NextWaitSemi = nullptr;
	// https://docs.vulkan.org/samples/latest/samples/performance/command_buffer_usage/README.html
	//vkResetCommandPool(QFAVKLogicalDevice::GetDevice(), commandPool, 0);
	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->minimized)
			continue;

		CurentProcessWindow = Windows[i];
		bool lastWindows = true; // search if nexts windows not minimized
		for (size_t j = i + 1; j < Windows.size(); j++)
		{// if Windows[i] last renderable windows not need use FinisSemiOtherWindow
			if (!Windows[j]->minimized)
			{
				lastWindows = false;
				break;
			}
		}

		Windows[i]->RenderWindow(lastWindows);		
	}

	PresentWindows();
	vkQueueWaitIdle(QFAVKLogicalDevice::GetGraphicsQueue());
}

void QFAWindow::RenderWindow(bool lastWindow)
{
	StartFrame();	
	for (size_t i = 0; i < Viewports.Length(); i++)
	{	
		BeginCommandBuffer();
		if (Viewports[i]->GetWorld())
		{
			ShadowRender(Viewports[i]);
			DrawActors(Viewports[i], i == 0, i);
		}

		DrawUI(Viewports[i], i);
		EndCommandBufferAndQueueSubmit();
		ViewportProcess++;
	}
}

void QFAWindow::PresentWindows()
{
	vkResetCommandBuffer(FinisCommandBuffer, 0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(FinisCommandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	static std::vector<VkSemaphore> finishSmiList;
	finishSmiList.clear();
	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->minimized)
			continue;

		Windows[i]->DrawOffscreenBuffer();		// draw offscreen buffer to swapchain buffer
		finishSmiList.push_back(Windows[i]->FinisSemaphore);
	}

	QueueSubmitPresent(finishSmiList); // submit all drawcall swapchain buffers

	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->minimized)
			continue;
		
		Windows[i]->PresentFrame(finishSmiList[i]);
	}
}


void QFAWindow::ProcessUIEvent()
{	
	for (size_t i = 0; i < Windows.size(); i++)
	{
		double x;
		double y;		
		if (!Windows[i]->GetMousePosition(x, y) || Windows[i]->minimized)
			continue;
		// don't replase int because in "i" can be minus value 
		for (int j = (int)Windows[i]->Viewports.Length() - 1; j >= 0; j--)
		{
			float xEnd = (float)(Windows[i]->Viewports[j]->X + Windows[i]->Viewports[j]->Width);
			float yEnd = (float)(Windows[i]->Viewports[j]->Y + Windows[i]->Viewports[j]->Height);
			if (x >= Windows[i]->Viewports[j]->X && y >= Windows[i]->Viewports[j]->Y &&
				x <= xEnd && y <= yEnd)
			{
				Windows[i]->UIEvent->NewFrame(&Windows[i]->Viewports[j]->Root, (float)x, (float)y, QTime::GetDeltaTime());
				return;
			}
		}

		Windows[i]->UIEvent->NewFrame(nullptr, (float)x, (float)y, QTime::GetDeltaTime());
	}
}


void QFAWindow::StartFrame()
{
	if (WindowSizeChanched)
	{
		recreateSwapChain();

		Width = NewWidth;
		Height = NewHeight;
		WindowSizeChanched = false;
		for (int j = 0; j < Viewports.Length(); j++)
			Viewports[j]->Settup(Width, Height);
	}

	VkResult result = vkAcquireNextImageKHR(QFAVKLogicalDevice::GetDevice(), SwapChain->SwapChain, UINT64_MAX, GetImageSemaphore, nullptr, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();	
		StartFrame();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		stopExecute("failed to acquire swap chain image!");	
}

void QFAWindow::PresentFrame(VkSemaphore finishSemi)
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; 
	
	
	VkSemaphore waitSemaphores[] = { GetImageSemaphore, finishSemi };
	presentInfo.waitSemaphoreCount = 2;
	presentInfo.pWaitSemaphores = waitSemaphores;

	VkSwapchainKHR swapChains[] = { SwapChain->SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;
	
	VkResult result = vkQueuePresentKHR(QFAVKLogicalDevice::GetPresentQueue(), &presentInfo);// Present frame
	

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || WindowSizeChanched)
	{
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		stopExecute("failed to present swap chain image!");
}

void QFAWindow::CreateShadow()
{
	ShadowSampler = new QFAVKTextureSampler();

	ShadowImage= new QFAImage( QFAVKShadowFrameBuffer::GetShadowResolution(), QFAVKShadowFrameBuffer::GetShadowResolution(),4, QFAWindow::depthFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	ShadowImagesView= new QFAVKImageView(ShadowImage, VK_IMAGE_ASPECT_DEPTH_BIT);
}



void QFAWindow::DrawActors(QFAViewport* _viewport, bool clear, int viewportIndex)
{
	CurentComandBuffer = ViewportStuff[ViewportProcess].comandBuffer;
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = clear ? frameBuffer->Clear.renderPass : frameBuffer->After.renderPass;
	renderPassInfo.framebuffer = frameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->swapChainExtent;


	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(CurentComandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 

	vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::Pipeline->GetPipeline());

	VkViewport viewport{};
	viewport.x = (float)_viewport->X;
	viewport.y = (float)_viewport->Y;
	viewport.width = (float)_viewport->Width;
	viewport.height = (float)_viewport->Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(CurentComandBuffer, 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapChain->swapChainExtent;
	vkCmdSetScissor(CurentComandBuffer, 0, 1, &scissor);


	QWorld* world = _viewport->GetWorld();
		
	glm::mat4 lightMat = world->GetDirectionDight()->GetLightMatrix();
	QMeshBaseComponent::StartFrameViewpoet(_viewport->MatrixPerspective,
		_viewport->CurentCamera->cameraRotationMatrex,
		lightMat, viewportIndex);
	unsigned int countComponentForRender = 0; 
	
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, false);

	vkCmdEndRenderPass(CurentComandBuffer);
}

unsigned int QFAWindow::ProcessMeshComponent(QSceneComponent* component, bool shadow)
{
	if (!component->IsValid())
		return 0;

	unsigned int countComponentForRender = 0;
	if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
	{		
		mesh->Render(CurentComandBuffer, shadow, CurentCameraPosition);
		countComponentForRender++;
	}

	for (size_t i = 0; i < component->ListComponents.Length(); i++)
		countComponentForRender += ProcessMeshComponent(component->ListComponents[i], shadow);

	return countComponentForRender;
}


void QFAWindow::CreateViewPortStuff()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SViewportStuff vs;

	if (vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &vs.semaphore.Actor) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &vs.semaphore.ActorShadow) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &vs.semaphore.Ui) != VK_SUCCESS)
		stopExecute("failed to create Semaphore");

	std::array<VkCommandBuffer, 1> comb;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = comb.size();

	if (vkAllocateCommandBuffers(QFAVKLogicalDevice::GetDevice(), &allocInfo, comb.data()) != VK_SUCCESS)
		stopExecute("failed to allocate command buffers!");

	vs.comandBuffer = comb[0];
	
	vs.buffers.uiProjectionBuffer = new QFAVKBuffer(sizeof(glm::mat4), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	vs.buffers.worldProjectionBuffer = new QFAVKBuffer(sizeof(QMeshBaseComponent::UBOVertex), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	vs.buffers.shadowBuffer = new QFAVKBuffer(sizeof(QMeshBaseComponent::UBOShadowVertex), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
	ViewportStuff.push_back(vs);

	QFAUIImage::CreateProjectionSet(vs.buffers.uiProjectionBuffer->Buffer);
	QFAText::CreateTextProjectionSet(vs.buffers.uiProjectionBuffer->Buffer);
	QMeshBaseComponent::createDescriptorSet0(vs.buffers.worldProjectionBuffer->Buffer, vs.buffers.shadowBuffer->Buffer);
}


