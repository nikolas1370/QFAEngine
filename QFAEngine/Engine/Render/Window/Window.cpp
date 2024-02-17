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


QFAWindow* QFAWindow::MainWindow = nullptr;

bool QFAWindow::Init = false;

VkSemaphore QFAWindow::imageAvailableSemaphore;
VkSemaphore QFAWindow::renderFinishedSemaphores;

VkSemaphore QFAWindow::imageAvailableSemaphoreMesh;
VkSemaphore QFAWindow::renderFinishedSemaphoresMesh;

VkFormat QFAWindow::depthFormat =  VK_FORMAT_D32_SFLOAT;

/*-----*/

QFAWindow::QFAWindow(int width, int height, std::string name)
{
	UIEvent.Init(this);
	
	if (!QFAWindow::MainWindow)
		QFAWindow::MainWindow = this;

	Width = width;
	Height = height;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(glfWindow, this);
	glfwSetFramebufferSizeCallback(glfWindow, framebufferResizeCallback);

	if (!glfWindow)
	{
		glfwTerminate();
		ASSERT(false);
		return;
	}

	if (!QFAWindow::Init)
	{
		QFAWindow::Init = true;
		glfwSetWindowSizeCallback(glfWindow, [](GLFWwindow* win, int w, int h)
			{
				if (QFAWindow::GetMainWindow()->glfWindow == win)
				{
					QFAWindow::GetMainWindow()->NewWidth = w;
					QFAWindow::GetMainWindow()->NewHeight = h;
					QFAWindow::GetMainWindow()->WindowSizeChanched = true;
				}
			});
	}

	Instance = new QFAVKInstance;
	if (glfwCreateWindowSurface(Instance->instance, glfWindow, nullptr, &surface) != VK_SUCCESS)
		stopExecute("QFAWindow::QFAWindow glfwCreateWindowSurface error");

	QFAVKPhysicalDevice::Init(Instance, surface);

	QFAVKLogicalDevice::Init(surface);
	QFAVKBuffer::Init(Instance->instance);
	
	


	

	createCommandPool();




	QFAImage::Init(commandPool);

	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	RenderPassSwapChain = new QFAVKRenderPassSwapChain(SwapChain->swapChainImageFormat, true);
	RenderPass = new QFAVKRenderPass(VK_FORMAT_R8G8B8A8_SRGB, true);
	
	RenderPassOffScreen = new QFAVKRenderPassDepth();
	TextRenderPass = new QFAVKTextRenderPass(VK_FORMAT_R8G8B8A8_SRGB);
	
	
	

	
	SwapChain->createFramebuffers(RenderPassSwapChain->renderPass);
	
	
	frameBufferMesh = new QFAVKMeshFrameBuffer(commandPool, width, height);
	
	
	CreateViewtortsBuffers();
	
	
	QFAText::Init(TextRenderPass->renderPass, commandPool);	



	
	QFAUIImage::Init(TextRenderPass->renderPass, commandPool);
	



	ShadowFrameBuffer = new QFAVKShadowFrameBuffer(commandPool, RenderPassOffScreen->renderPass);


	CreateShadow();
	
	QMeshBaseComponent::Init(RenderPass->renderPass, RenderPassOffScreen->renderPass, commandPool);

	

	createCommandBuffer();
	createSyncObject();
	
	imugo = new QFAPresentImage(commandPool);

	imugo->Init(RenderPassSwapChain->renderPass, commandPool, frameBufferMesh->ColorImage);


	QFAViewport* vp = new QFAViewport;	
	AddViewport(vp);
	vp->Settup(width, height);
}

QFAWindow::~QFAWindow()
{
	/*
	delete all
	
	*/


	glfwDestroyWindow(glfWindow);
}

void QFAWindow::createCommandBuffer()
{
	std::array<VkCommandBuffer, 2 * MaxActiveViewPort + 2> comb;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)comb.size();

	if (vkAllocateCommandBuffers(QFAVKLogicalDevice::GetDevice(), &allocInfo, comb.data()) != VK_SUCCESS)
		stopExecute("failed to allocate command buffers!");

	FinisCommandBuffer = comb[comb.size() - 2];
	UICommandBuffer = comb[comb.size() - 1];
	int index = 0;
	for (size_t i = 0; i < MaxActiveViewPort; i += 2)
	{
		ShadowCommandBuffers[index] = comb[i];
		MeshCommandBuffers[index] = comb[i + 1];		
		index++;
	}
}

void QFAWindow::DrawUI()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->UICommandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->TextRenderPass->renderPass;
	renderPassInfo.framebuffer = MainWindow->frameBufferMesh->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = MainWindow->SwapChain->swapChainExtent;

	std::array<VkClearValue, 3> clearValues{};
	clearValues[0].color = { {0, 0, 0, 0} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->UICommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	/*---*/
	QFAText::StartTextRender();
	QFAVKPipeline* pipeline = nullptr;

	for (int u = 0; u < Viewports.Length(); u++)
	{
		VkViewport viewport{};
		viewport.x = (float)Viewports[u]->X;
		viewport.y = (float)Viewports[u]->Y;
		viewport.width = (float)Viewports[u]->Width;
		viewport.height = (float)Viewports[u]->Height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(UICommandBuffer, 0, 1, &viewport); // can in start render viewport do 

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = SwapChain->swapChainExtent;
		vkCmdSetScissor(UICommandBuffer, 0, 1, &scissor);

		StartUIRenderViewPort(u);

		SortUIs(&Viewports[u]->Root); // parent should add before children
		for (size_t i = 0; i < SortUIUnits.Length(); i++)
		{			
			if (SortUIUnits[i]->CanRender)
			{
				if (pipeline != ((QFAUIRenderUnit*)SortUIUnits[i])->GetPipeline())
				{
					pipeline = ((QFAUIRenderUnit*)SortUIUnits[i])->GetPipeline();
					vkCmdBindPipeline(UICommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
				}

				((QFAUIRenderUnit*)SortUIUnits[i])->Render(UICommandBuffer);
			}
			else if (SortUIUnits[i]->CanBeParent)
			{
				if (pipeline != ((QFAUIParent*)SortUIUnits[i])->GetBackgroundPipeline())
				{
					pipeline = ((QFAUIParent*)SortUIUnits[i])->GetBackgroundPipeline();
					vkCmdBindPipeline(UICommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
				}

				((QFAUIParent*)SortUIUnits[i])->RenderBackground(UICommandBuffer);
			}
		}
	}	


	/*---*/

	vkCmdEndRenderPass(MainWindow->UICommandBuffer);


	if (vkEndCommandBuffer(MainWindow->UICommandBuffer) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = MainWindow->ViewportProcess;
	submitInfo.pWaitSemaphores = MainWindow->ActorFinishedSemi.data();	
	submitInfo.pWaitDstStageMask = ActorFinishedSemiFlags.data();

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &MainWindow->UICommandBuffer;

	VkSemaphore signalSemaphores[] = { MainWindow->UISemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");
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
		if (parent->OneUnit)
			AddUnit(((QFAUIParentOneUnit*)unit)->Child);
		else
		{
			QFAUIParentMultipleUnit* parentMultiple = (QFAUIParentMultipleUnit*)unit;
			for (size_t i = 0; i < parentMultiple->Children.Length(); i++)
				AddUnit(parentMultiple->Children[i]);
		}
	}	
}

void QFAWindow::StartUIRenderViewPort( int viewportIndex)
{	
	QFAText::CurentDescriptorSetProject = QFAText::Pipeline->GetSet(0, viewportIndex);	
	QFAUIImage::CurentDescriptorSetProject = QFAUIImage::Pipeline->GetSet(0, viewportIndex);

	UniformBufferObject ubo{};
	ubo.projection = MainWindow->Viewports[viewportIndex]->UIProjection;
	memcpy(ViewportBuffers[viewportIndex].uiProjectionBuffer->MapData, &ubo, sizeof(ubo.projection));

	
}

void QFAWindow::DrawOffscreenBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->FinisCommandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->RenderPassSwapChain->renderPass;
	renderPassInfo.framebuffer = SwapChain->swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = MainWindow->SwapChain->swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->FinisCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)Width;
	viewport.height = (float)Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->FinisCommandBuffer, 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = MainWindow->SwapChain->swapChainExtent;
	vkCmdSetScissor(MainWindow->FinisCommandBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(MainWindow->FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, QFAPresentImage::Pipeline->GetPipeline());

	recordCommandBufferTestImege();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &UISemaphore;// ;
	
	
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &FinisCommandBuffer;

	VkSemaphore signalSemaphores[] = { FinisSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkCmdEndRenderPass(FinisCommandBuffer);

	if (vkEndCommandBuffer(FinisCommandBuffer) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");
	
	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");	
}

void QFAWindow::recordCommandBufferTestImege()
{
	VkBuffer vertexBuffers[] = { imugo->vertexBufer->GpuSideBuffer->Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(FinisCommandBuffer, 0, 1, vertexBuffers, offsets);

	VkDescriptorSet descSet = QFAPresentImage::Pipeline->GetSet(0, 0);

	vkCmdBindDescriptorSets(FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		QFAPresentImage::Pipeline->GetPipelineLayout(), 0, 1, &descSet , 0, nullptr);
	
	vkCmdDraw(FinisCommandBuffer, static_cast<uint32_t>(6), 1, 0, 0);
}

void QFAWindow::createSyncObject()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;	
	
	if(vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &GetImageSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &UISemaphore) != VK_SUCCESS)
		stopExecute("failed to create Semaphore");

	for (size_t i = 0; i < QFAWindow::MaxActiveViewPort; i++)
	{
		ActorFinishedSemiFlags[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &ActorFinishedSemi[i]) != VK_SUCCESS ||
			vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &ActorShadowFinishedSemi[i]) != VK_SUCCESS)
			stopExecute("failed to create Semaphore");
	}

}

void QFAWindow::recreateSwapChain()
{
	delete SwapChain;
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	SwapChain->createFramebuffers(RenderPass->renderPass);

	frameBufferMesh->ResizeBuffer(commandPool, NewWidth, NewHeight);
	delete imugo;
	imugo = new QFAPresentImage(commandPool);
	imugo->Init(RenderPass->renderPass, commandPool, frameBufferMesh->ColorImage);
}

void QFAWindow::AddViewport(QFAViewport* viewport)
{
	Viewports.Add(viewport);
	viewport->Settup(Width, Height);
	viewport->WindowAddMe(this);
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




void QFAWindow::ShadowRender(QFAViewport* _viewport)
{
	vkResetCommandBuffer(MainWindow->ShadowCommandBuffers[ViewportProcess], 0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->ShadowCommandBuffers[ViewportProcess], &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->RenderPassOffScreen->renderPass;
	renderPassInfo.framebuffer = ShadowFrameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { (uint32_t)shadowResolution, (uint32_t)shadowResolution };

	std::array<VkClearValue, 1> clearValues{};
	clearValues[0].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->ShadowCommandBuffers[ViewportProcess], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Set depth bias (aka "Polygon offset")
// Required to avoid shadow mapping artifacts
	vkCmdSetDepthBias(
		MainWindow->ShadowCommandBuffers[ViewportProcess],
		depthBiasConstant,
		0.0f,
		depthBiasSlope);

	vkCmdBindPipeline(MainWindow->ShadowCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::ShadowPipline->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = shadowResolution;
	viewport.height = shadowResolution;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->ShadowCommandBuffers[ViewportProcess], 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = { (uint32_t)shadowResolution, (uint32_t)shadowResolution };
	vkCmdSetScissor(MainWindow->ShadowCommandBuffers[ViewportProcess], 0, 1, &scissor);

	QWorld* world = _viewport->CurentCamera->GetWorld();
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
	

	vkCmdEndRenderPass(ShadowCommandBuffers[ViewportProcess]);

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

	vkCmdCopyImage(MainWindow->ShadowCommandBuffers[ViewportProcess], ShadowFrameBuffer->depthImageQFA->TextureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ShadowImage->TextureImage, VK_IMAGE_LAYOUT_GENERAL, 1, &ic);

	if (vkEndCommandBuffer(ShadowCommandBuffers[ViewportProcess]) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &ShadowCommandBuffers[ViewportProcess];

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &ActorShadowFinishedSemi[ViewportProcess];



	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr/*inFlightFence*/) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");

}


void QFAWindow::RenderWindow()
{
	MainWindow->ViewportProcess = 0;

	MainWindow->StartFrame();
	QMeshBaseComponent::StartFrame();
	for (size_t i = 0; i < MainWindow->Viewports.Length(); i++)
	{		
		MainWindow->ShadowRender(MainWindow->Viewports[i]);

		MainWindow->DrawActors(MainWindow->Viewports[i], i == 0);
				
		MainWindow->ViewportProcess++;
	}
	


	
	MainWindow->DrawUI();

	MainWindow->DrawOffscreenBuffer();

	
	MainWindow->PresentFrame();
	
}

void QFAWindow::ProcessUIEvent()
{	

	

	double x;
	double y;
	glfwGetCursorPos(MainWindow->glfWindow, &x, &y);
	// don't replase int because in "i" can be minus value 
	for (int i = (int)MainWindow->Viewports.Length() - 1; i >= 0; i--)
	{
		float xEnd = (float)(MainWindow->Viewports[i]->X + MainWindow->Viewports[i]->Width);
		float yEnd = (float)(MainWindow->Viewports[i]->Y + MainWindow->Viewports[i]->Height);
		if (x >= MainWindow->Viewports[i]->X && y >= MainWindow->Viewports[i]->Y &&
			x <= xEnd && y <= yEnd )
		{
			MainWindow->UIEvent.NewFrame(&MainWindow->Viewports[i]->Root, (float)x, (float)y, QTime::GetDeltaTime());
			return;
		}
	}

	MainWindow->UIEvent.NewFrame(nullptr, (float)x, (float)y, QTime::GetDeltaTime());
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

void QFAWindow::PresentFrame()
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; 
	
	VkSemaphore waitSemaphores[] = { GetImageSemaphore, FinisSemaphore};
	presentInfo.waitSemaphoreCount = 2;
	presentInfo.pWaitSemaphores = waitSemaphores;

	VkSwapchainKHR swapChains[] = { SwapChain->SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;
	
	VkResult result = vkQueuePresentKHR(QFAVKLogicalDevice::GetPresentQueue(), &presentInfo);// Present frame
	

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) 
	{
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		stopExecute("failed to present swap chain image!");

	vkQueueWaitIdle(QFAVKLogicalDevice::GetGraphicsQueue());	
}

void QFAWindow::CreateShadow()
{
	ShadowSampler = new QFAVKTextureSampler();

	ShadowImage= new QFAImage( QFAVKShadowFrameBuffer::GetShadowResolution(), QFAVKShadowFrameBuffer::GetShadowResolution(),4, QFAWindow::depthFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	ShadowImagesView= new QFAVKImageView(ShadowImage, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void QFAWindow::CreateViewtortsBuffers()
{	
	for (size_t i = 0; i < ViewportBuffers.size(); i++)
		ViewportBuffers[i].uiProjectionBuffer = new QFAVKBuffer(sizeof(glm::mat4), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

void QFAWindow::DrawActors(QFAViewport* _viewport, bool clear)
{
	vkResetCommandBuffer(MainWindow->MeshCommandBuffers[ViewportProcess], /*VkCommandBufferResetFlagBits*/ 0);
	
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->MeshCommandBuffers[ViewportProcess], &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = clear ? frameBufferMesh->Clear.renderPass : frameBufferMesh->After.renderPass;
	renderPassInfo.framebuffer = frameBufferMesh->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = MainWindow->SwapChain->swapChainExtent;


	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->MeshCommandBuffers[ViewportProcess], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(MainWindow->MeshCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::Pipeline->GetPipeline());

	VkViewport viewport{};
	viewport.x = (float)_viewport->X;
	viewport.y = (float)_viewport->Y;
	viewport.width = (float)_viewport->Width;
	viewport.height = (float)_viewport->Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->MeshCommandBuffers[ViewportProcess], 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = MainWindow->SwapChain->swapChainExtent;
	vkCmdSetScissor(MainWindow->MeshCommandBuffers[ViewportProcess], 0, 1, &scissor);


	QWorld* world = _viewport->CurentCamera->GetWorld();
		
	glm::mat4 lightMat = world->GetDirectionDight()->GetLightMatrix();
	QMeshBaseComponent::StartFrameViewpoet(_viewport->MatrixPerspective,
		_viewport->CurentCamera->cameraRotationMatrex,
		lightMat); 
	unsigned int countComponentForRender = 0; 
	
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, false);

	vkCmdEndRenderPass(MeshCommandBuffers[ViewportProcess]);

	if (vkEndCommandBuffer(MeshCommandBuffers[ViewportProcess]) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &ActorShadowFinishedSemi[ViewportProcess];
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &MeshCommandBuffers[ViewportProcess];


	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &ActorFinishedSemi[ViewportProcess];

	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr/*MainWindow->inFlightFence*/) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");

	
}

unsigned int QFAWindow::ProcessMeshComponent(QSceneComponent* component, bool shadow)
{
	if (!component->IsValid())
		return 0;

	unsigned int countComponentForRender = 0;
	if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
	{		
		mesh->UpdateBuffers(0, shadow, CurentCameraPosition);
		recordCommandBufferMesh(mesh, shadow);
		countComponentForRender++;
	}

	for (size_t i = 0; i < component->ListComponents.Length(); i++)
		countComponentForRender += ProcessMeshComponent(component->ListComponents[i], shadow);

	return countComponentForRender;
}


void QFAWindow::recordCommandBufferMesh(QMeshBaseComponent* mesh, bool shadow)
{	
	VkDeviceSize offsets[] = { 0 };
	if (shadow)
	{
		vkCmdBindVertexBuffers(ShadowCommandBuffers[ViewportProcess], 0, 1, &mesh->VertexBufer->GpuSideBuffer->Buffer, offsets);
		vkCmdBindIndexBuffer(ShadowCommandBuffers[ViewportProcess], mesh->IndexBuffer->GpuSideBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);
		auto nextSet = mesh->GetShadowNextSet();
		vkCmdBindDescriptorSets(ShadowCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS,
			QMeshBaseComponent::ShadowPipline->GetPipelineLayout(), 0, 1, &nextSet, 0, nullptr);

		vkCmdDrawIndexed(ShadowCommandBuffers[ViewportProcess], static_cast<uint32_t>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}
	else
	{
		vkCmdBindVertexBuffers(MeshCommandBuffers[ViewportProcess], 0, 1, &mesh->VertexBufer->GpuSideBuffer->Buffer, offsets);
		vkCmdBindIndexBuffer(MeshCommandBuffers[ViewportProcess], mesh->IndexBuffer->GpuSideBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);

		auto nextSets = mesh->GetNextSets();
		vkCmdBindDescriptorSets(MeshCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS,
			QMeshBaseComponent::Pipeline->GetPipelineLayout(), 0, (uint32_t)nextSets.size(), nextSets.data(), 0, nullptr);

		vkCmdDrawIndexed(MeshCommandBuffers[ViewportProcess], static_cast<uint32_t>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}	
}



