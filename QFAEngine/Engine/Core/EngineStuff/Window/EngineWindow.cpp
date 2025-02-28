﻿#include "pch.h"

#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Core/Time.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Core/EngineStuff/Window/ViewportHolder.h>
#include <Object/Actor/Actor.h>
#include <UI/UIImage.h>
#include <UI/Text.h>
#include <Core/EngineStuff/RenderPass/RenderPassSwapChain.h>
#include <Core/EngineStuff/Framebuffer/MeshFrameBuffer.h> 
#include <UI/UIParentMultipleUnit.h>
#include <Core/EngineStuff/Pipline/Pipline.h>
#include <UI/UIParentOneUnit.h>
#include <UI/UIParentHiddenChild.h>
#include <Core/EngineStuff/Window/UIEvent.h>
#include <Core/EngineStuff/Window/EngineViewport.h>
#include <UI/UIUnit.h>
#include <Core/EngineStuff/vk/PresentImage.h>
#include <Core/EngineStuff/RenderPass/TextRenderPass.h>
#include <Core/EngineStuff/Framebuffer/ShadowFrameBuffer.h>
#include <Core/EngineStuff/RenderPass/RenderPassDepth.h>
#include <Core/EngineStuff/vk/ImageView.h>
#include <Core/EngineStuff/vk/TextureSampler.h>
#include <Core/EngineStuff/Image.h>
#include <Core/EngineStuff/vk/QueueFamilies.h>
#include <Core/EngineStuff/vk/SwapChain.h>
#include <Core/EngineStuff/vk/LogicalDevice.h>
#include <Core/EngineStuff/vk/VKInstance.h>
#include <GLFW/glfw3.h>
#include <Core/Window/Viewport.h>

VkCommandPool QFAEngineWindow::commandPool;

QFAVKShadowFrameBuffer* QFAEngineWindow::ShadowFrameBuffer;
QFAVKImageView* QFAEngineWindow::ShadowImagesView;
QFAVKTextureSampler* QFAEngineWindow::ShadowSampler;

std::vector<QFAEngineWindow::SViewportStuff> QFAEngineWindow::ViewportStuff;
int QFAEngineWindow::ViewportProcess;
VkSemaphore* QFAEngineWindow::NextWaitSemi;
QFAEngineWindow* QFAEngineWindow::CurentProcessWindow;


QFAVKRenderPassSwapChain* QFAEngineWindow::RenderPassSwapChain;
QFAVKRenderPassDepth* QFAEngineWindow::RenderPassOffScreen;
QFAVKTextRenderPass* QFAEngineWindow::TextRenderPass;
QFAVKTextRenderPass* QFAEngineWindow::TextRenderPassClear;


VkFormat QFAEngineWindow::depthFormat =  VK_FORMAT_D32_SFLOAT;
std::vector<QFAEngineWindow*> QFAEngineWindow::Windows;


QFAVKInstance* QFAEngineWindow::Instance = nullptr;
QFAImage* QFAEngineWindow::ShadowImage = nullptr;

VkCommandBuffer QFAEngineWindow::FinisCommandBuffer;
/*-----*/


QFAEngineWindow::QFAEngineWindow(int width, int height, std::string name, bool inCenter, bool decorated, std::function<void()> closedFun)
{
	ClosedFun = closedFun;
	Windows.push_back(this);
	Width = width;
	Height = height;

	QFAEngineWindow::CurentProcessWindow = this;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	if(inCenter)
	{
		GLFWmonitor* pm = glfwGetPrimaryMonitor();
		int x, y, w, h;
		glfwGetMonitorWorkarea(pm, &x, &y, &w, &h);
		glfwWindowHint(GLFW_POSITION_X, w / 2 - Width / 2 + x);
		glfwWindowHint(GLFW_POSITION_Y, h / 2 - Height / 2 + y);
	}

	if (decorated)
		glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	else
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	glfWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
	QFAInput::WindowCreated(this);
	UIEvent = new QFAUIEvent(this, glfWindow);
	glfwSetWindowUserPointer(glfWindow, this);	

	if (!glfWindow)
	{
		glfwTerminate();
		ASSERT(false);
		return;
	}

	glfwSetWindowSizeCallback(glfWindow, [](GLFWwindow* win, int w, int h)
	{		
		for (size_t i = 0; i < QFAEngineWindow::Windows.size(); i++)
		{
			if (QFAEngineWindow::Windows[i]->glfWindow == win)
			{		
				if (w == 0 || h == 0)
				{
					QFAEngineWindow::Windows[i]->minimized = true;
					return;
				}
				else
					QFAEngineWindow::Windows[i]->minimized = false;

				QFAEngineWindow::Windows[i]->NewWidth = w;
				QFAEngineWindow::Windows[i]->NewHeight = h;
				QFAEngineWindow::Windows[i]->WindowSizeChanched = true;
				return;
			}
		}
	});

	glfwSetCursorEnterCallback(glfWindow, [](GLFWwindow* window, int entered)
	{
		for (size_t i = 0; i < QFAEngineWindow::Windows.size(); i++)
		{
			if (QFAEngineWindow::Windows[i]->glfWindow == window)
			{
				QFAEngineWindow::Windows[i]->CursorOnWindow = entered;
				return;
			}
		}			
	});	

	glfwSetDropCallback(glfWindow, [](GLFWwindow* window, int path_count, const char* paths[])
		{
#if QFA_EDITOR_ONLY
			if (QFAEngineViewport::GetInGame())
				return;
#endif

			for (size_t i = 0; i < QFAEngineWindow::Windows.size(); i++)
			{
				if (QFAEngineWindow::Windows[i]->glfWindow == window)
				{
					if (QFAEngineWindow::Windows[i]->DropFunction)
						QFAEngineWindow::Windows[i]->DropFunction(path_count, paths);

					return;
				}
			}
		});

	if(!Instance)
		Instance = new QFAVKInstance;

	if (glfwCreateWindowSurface(Instance->instance, glfWindow, nullptr, &surface) != VK_SUCCESS)
		stopExecute("QFAEngineWindow::QFAEngineWindow glfwCreateWindowSurface error");

	if (Windows.size() == 1)
	{
		QFAVKPhysicalDevice::Init(Instance, surface);
		QFAVKLogicalDevice::Init(surface);
		QFAVKBuffer::Init(Instance->instance);
		createCommandPool();
	}
	
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	if (Windows.size() == 1)
	{
		RenderPassOffScreen = new QFAVKRenderPassDepth();
		TextRenderPass = new QFAVKTextRenderPass(imageFormat, false);
		TextRenderPassClear = new QFAVKTextRenderPass(imageFormat, true);
		RenderPassSwapChain = new QFAVKRenderPassSwapChain(SwapChain->swapChainImageFormat, true);
	
		QFAText::Init(TextRenderPass->renderPass, commandPool);
		QFAUIImage::Init(TextRenderPass->renderPass, commandPool);

		ShadowFrameBuffer = new QFAVKShadowFrameBuffer(commandPool, RenderPassOffScreen->renderPass);
		CreateShadow();

		createCommandBuffer();
	}

	SwapChain->createFramebuffers(RenderPassSwapChain->renderPass);	
	frameBuffer = new QFAVKMeshFrameBuffer(commandPool, width, height, imageFormat);
	PresentImage = new QFAPresentImage(commandPool, RenderPassSwapChain->renderPass, frameBuffer->ColorImage);

	if (Windows.size() == 1)
		QMeshBaseComponent::Init(frameBuffer->Clear.renderPass, RenderPassOffScreen->renderPass, commandPool);
	

	createSyncObject();
	
	QFAViewport* vp = new QFAViewport;	
	AddViewport(vp);
	vp->Settup(width, height);
	
	PickBuffer = new QFAVKBuffer(4, nullptr, true, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}

QFAEngineWindow::QFAEngineWindow()
{
	Windows.push_back(this);

}

void QFAEngineWindow::InitNotRegularWindow(QFAEngineWindow* parentWidow)
{	
	ParentWindow = parentWidow;
	ParentWindow->AddChildWindow(this);
	QFAInput::WindowCreated(this);
	glfWindow = parentWidow->glfWindow;// only for QFAInput
	UIEvent = new QFAUIEvent(this, nullptr);
}

QFAEngineWindow::~QFAEngineWindow()
{
	for (size_t i = 0; i < Viewports.size(); i++)
		Viewports[i]->WindowRemoveMe();

	delete UIEvent;
	QFAInput::WindowClosed(this);

	if(RegularWindow)
		glfwDestroyWindow(glfWindow);
	else if (ParentWindow)
		ParentWindow->RemoveChildWindow(this);	

	if (ClosedFun)
		ClosedFun();
}

void QFAEngineWindow::SetClosedFun(std::function<void()> fun)
{
	ClosedFun = fun;
}

void QFAEngineWindow::createCommandBuffer()
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

void QFAEngineWindow::DrawUI(QFAEngineViewport* viewport, bool clear)
{
	CurentComandBuffer = ViewportStuff[ViewportProcess].comandBuffer;
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; // if in first viewport not render mesh, text should clear buffer
	renderPassInfo.renderPass = clear ? TextRenderPassClear->renderPass : TextRenderPass->renderPass;
	renderPassInfo.framebuffer = frameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->swapChainExtent;

	std::array<VkClearValue, 3> clearValues{};
	clearValues[0].color = { {0, 0, 0, 0} };
	clearValues[1].color = { {0.0, 0.0, 0.0, 0.0} };
	clearValues[2].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(CurentComandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 
	/*---*/
	QFAVKPipeline* pipeline = nullptr;



	VkViewport vkViewport{};
	FVector2D viewPos = viewport->GetPositionRelativeRegularWindow();
	vkViewport.x = viewPos.X;
	vkViewport.y = viewPos.Y;
	vkViewport.width = (float)viewport->Width;
	vkViewport.height = (float)viewport->Height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;
	vkCmdSetViewport(CurentComandBuffer, 0, 1, &vkViewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = SwapChain->swapChainExtent;
	vkCmdSetScissor(CurentComandBuffer, 0, 1, &scissor);

	StartUIRenderViewPort(viewport);

	SortUIs(&viewport->Root); // parent should add before children
#if QFA_EDITOR_ONLY
	size_t len = 0;
	if (viewport->Window->RegularWindow || QFAViewport::InGame) // ui be rendered if viewport is RegularWindow or game started
		len = SortUIUnits.Length();
#else
	size_t len = SortUIUnits.Length();
#endif

	for (size_t i = 0; i < len; i++)

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
			pipeline = ((QFAUIParent*)SortUIUnits[i])->GetBackgroundPipeline();
			vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());
			((QFAUIParent*)SortUIUnits[i])->RenderBackground(CurentComandBuffer);
		}
	}
	
	vkCmdEndRenderPass(CurentComandBuffer);

}

void QFAEngineWindow::SortUIs(QFAViewportRoot* root)
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

void QFAEngineWindow::AddUnit(QFAUIUnit* unit)
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
		else if (parent->GetParentType() == QFAUIParent::EParentType::HiddenChild)
		{
			QFAParentHiddenChild* parentHiden = (QFAParentHiddenChild*)unit;
			for (size_t i = 0; i < parentHiden->Children.size(); i++)
				AddUnit(parentHiden->Children[i]);
		}
		else  if (parent->GetParentType() == QFAUIParent::EParentType::MultipleChild)
		{
			QFAUIParentMultipleUnit* parentMultiple = (QFAUIParentMultipleUnit*)unit;
			for (size_t i = 0; i < parentMultiple->Children.Length(); i++)
				AddUnit(parentMultiple->Children[i]);
		}
	}	
}

void QFAEngineWindow::AddChildWindow(QFAEngineWindow* win)
{
	WindowChildCallback.push_back({ win });
}

void QFAEngineWindow::RemoveChildWindow(QFAEngineWindow* childWin)
{
	for (size_t i = 0; i < WindowChildCallback.size(); i++)
	{
		if (WindowChildCallback[i].ChileWindow == childWin)
		{
			WindowChildCallback.erase(WindowChildCallback.begin() + i);
			break;
		}
	}
}

void QFAEngineWindow::AddCharCallback(QFAEngineWindow* childWin, std::function<void(GLFWwindow*, unsigned int)> callback)
{
	for (size_t i = 0; i < WindowChildCallback.size(); i++)
	{
		if (WindowChildCallback[i].ChileWindow == childWin)
		{
			WindowChildCallback[i].callback = callback;
			return;
		}
	}

	stopExecute("before use this functin need add window by AddChildWindow ");
}

void QFAEngineWindow::ProcessGetMeshId()
{
	double x, y;
	for (size_t i = 0; i < Windows.size(); i++)
	{		
		if (Windows[i]->GetMeshCallback && Windows[i]->GetMousePosition(x, y))
		{
			if (x > 0 && y > 0)
			{
				Windows[i]->PickBuffer->copyInBuffer(Windows[i]->frameBuffer->IdImage, 1, 1, commandPool, x - 1, y - 1, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_GENERAL);
				unsigned int rawChar = *(unsigned int*)Windows[i]->PickBuffer->MapData;
				if (rawChar)
					Windows[i]->GetMeshCallback(QMeshBaseComponent::MeshIdList[rawChar - 1]);
			}
		}

		Windows[i]->GetMeshCallback = nullptr;
	}	
}

void QFAEngineWindow::GetMeshUnderCursore(std::function<void(QMeshBaseComponent*)> callback)
{
	GetMeshCallback = callback;
}

void QFAEngineWindow::SetWindowTitle(const char* winName)
{
	glfwSetWindowTitle(glfWindow, winName);
}

void QFAEngineWindow::StartUIRenderViewPort(QFAEngineViewport* viewport)
{	
	QFAText::CurentDescriptorSetProject = QFAText::Pipeline->GetSet(0, QFAEngineWindow::ViewportProcess);
	QFAUIImage::CurentDescriptorSetProject = QFAUIImage::Pipeline->GetSet(0, QFAEngineWindow::ViewportProcess);
	
	UniformBufferObject ubo{};
	ubo.projection = viewport->UIProjection;
	memcpy(ViewportStuff[QFAEngineWindow::ViewportProcess].buffers.uiProjectionBuffer->MapData, &ubo, sizeof(ubo.projection));
}

void QFAEngineWindow::DrawOffscreenBuffer()
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

void QFAEngineWindow::recordCommandBufferTestImege()
{
	VkBuffer vertexBuffers[] = { PresentImage->vertexBufer->GpuSideBuffer->Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(FinisCommandBuffer, 0, 1, vertexBuffers, offsets);

	VkDescriptorSet descSet = PresentImage->Pipeline->GetSet(0, 0);
	
	vkCmdBindDescriptorSets(FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		PresentImage->Pipeline->GetPipelineLayout(), 0, 1, &descSet , 0, nullptr);
	
	vkCmdDraw(FinisCommandBuffer, static_cast<uint32_t>(6), 1, 0, 0); 
	
}

void QFAEngineWindow::QueueSubmitPresent(std::vector<VkSemaphore>& listSemi)
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

void QFAEngineWindow::createSyncObject()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;	
	
	if(vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &GetImageSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemaphore) != VK_SUCCESS || 
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemiOtherWindow) != VK_SUCCESS)
		stopExecute("failed to create Semaphore");
}

void QFAEngineWindow::recreateSwapChain()
{
	delete SwapChain;
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);	
	SwapChain->createFramebuffers(RenderPassSwapChain->renderPass);
	
	frameBuffer->ResizeBuffer(commandPool, NewWidth, NewHeight);
	delete PresentImage;
	PresentImage = new QFAPresentImage(commandPool, RenderPassSwapChain->renderPass, frameBuffer->ColorImage);	
}

void QFAEngineWindow::AddViewport(QFAViewport* viewport)
{
	viewport->Window = (QFAWindow*)this;
	Viewports.push_back(viewport);
	viewport->WindowAddMe((QFAWindow*)this);
	viewport->Settup(Width, Height);

	int activeViewportCount = 0;
	for (size_t i = 0; i < Windows.size(); i++)
		activeViewportCount += Windows[i]->Viewports.size();

	if (ViewportStuff.size() < activeViewportCount)
		CreateViewPortStuff();
}

void QFAEngineWindow::RemoveViewport(QFAViewport* viewport)
{
	if (Viewports.size() == 1)
		return;

	for (size_t i = 0; i < Viewports.size(); i++)
	{
		if (Viewports[i] == viewport)
		{
			Viewports.erase(Viewports.begin() + i);;
			viewport->WindowRemoveMe();
			break;
		}
	}
}

QFAViewport* QFAEngineWindow::GetViewport(size_t index)
{
	if (index >= Viewports.size())
		return nullptr;
	else
		return (QFAViewport*)Viewports[index];
}

bool QFAEngineWindow::GetMousePosition(double& x, double& y)
{
#if QFA_EDITOR_ONLY
	if (RegularWindow)
	{
#endif
		glfwGetCursorPos(glfWindow, &x, &y);
		return CursorOnWindow;
#if QFA_EDITOR_ONLY
	}
	else
	{
		QFAWindow* win = (QFAWindow*)this;
		if (!win->Holder->Window->GetMousePosition(x, y))
		{
			x -= win->Holder->X;
			y -= win->Holder->Y;
			return false;
		}

		if (win->Holder->X <= x && win->Holder->X + win->Holder->Width >= x &&
			win->Holder->Y <= y && win->Holder->Y + win->Holder->Height >= y)
		{
			x -= win->Holder->X;
			y -= win->Holder->Y;
			return true;
		}
		else
		{
			x -= win->Holder->X;
			y -= win->Holder->Y;
			return false;
		}
	}
#endif
}

void QFAEngineWindow::createCommandPool()
{ 
	QFAVKStruct::QueueFamilyIndices queueFamilyIndices = QFAVKQueueFamilies::findQueueFamilies(QFAVKPhysicalDevice::GetDevice(), surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	
	if (vkCreateCommandPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		stopExecute("failed to create graphics command pool!");
}

void QFAEngineWindow::BeginCommandBuffer()
{
	vkResetCommandBuffer(ViewportStuff[ViewportProcess].comandBuffer, 0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(ViewportStuff[ViewportProcess].comandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");
}

void QFAEngineWindow::EndCommandBufferAndQueueSubmit()
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

void QFAEngineWindow::ShadowRender(QFAEngineViewport* _viewport)
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
	
	QMeshBaseComponent::ShaderDL.Ambient = dl->Ambient;
	QMeshBaseComponent::ShaderDL.Diffuse = dl->Diffuse;
	QMeshBaseComponent::ShaderDL.Direction = dl->Direction;
	QMeshBaseComponent::ShaderDL.Specular = dl->Specular;
	
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

void QFAEngineWindow::RenderWindows()
{ 
	ViewportProcess = 0;
	
	QMeshBaseComponent::StartFrame();
	QFAText::StartFrame();
	
	NextWaitSemi = nullptr;
	// https://docs.vulkan.org/samples/latest/samples/performance/command_buffer_usage/README.html
	//vkResetCommandPool(QFAVKLogicalDevice::GetDevice(), commandPool, 0);

	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->minimized || !Windows[i]->RegularWindow)
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
}

void QFAEngineWindow::RenderWindow(bool lastWindow)
{
	bool clear = true;
	StartFrame();	
	for (size_t i = 0; i < Viewports.size(); i++)
	{			
#if QFA_EDITOR_ONLY		
		if (Viewports[i]->RegularViewport)
			RenderViewport(Viewports[i], i, clear);
		else
		{			
			QFAViewportHolder* viewHold = (QFAViewportHolder*)Viewports[i];
			for (size_t j = 0; j < viewHold->CurentHoldedWindow->Viewports.size(); j++)
				RenderViewport(viewHold->CurentHoldedWindow->Viewports[j], i + j, clear);
		}
#else
		RenderViewport(Viewports[i], i, clear);
#endif
	}
}

void QFAEngineWindow::RenderViewport(QFAEngineViewport* viewport, size_t i, bool& clear)
{
	BeginCommandBuffer();
	if (viewport->GetWorld())
	{
		clear = false;
		ShadowRender(viewport);
		DrawActors(viewport, i == 0);
	}

	DrawUI(viewport, clear);
	clear = false;
	EndCommandBufferAndQueueSubmit();
	ViewportProcess++;
}

void QFAEngineWindow::PresentWindows()
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
		if (Windows[i]->minimized || !Windows[i]->RegularWindow)
			continue;

		Windows[i]->DrawOffscreenBuffer();// draw offscreen buffer to swapchain buffer
		finishSmiList.push_back(Windows[i]->FinisSemaphore);
	}
	
	QueueSubmitPresent(finishSmiList); // submit all drawcall swapchain buffers
	
	int finishSmiIndex = 0;
	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->minimized || !Windows[i]->RegularWindow)
			continue;

		Windows[i]->PresentFrame(finishSmiList[finishSmiIndex++]);
	}
}


void QFAEngineWindow::ProcessUIEvent()
{	
	for (size_t i = 0; i < Windows.size(); i++)
		if(!Windows[i]->ShouldClose())
			Windows[i]->ProcessUIEventInside();
}

void QFAEngineWindow::ProcessUIEventInside()
{
	if (!RegularWindow && !QFAEngineViewport::GetInGame())
		return;

	double x;
	double y;
	if (!GetMousePosition(x, y))
	{
		if (UIEvent->FocusUnit)
		{
			UIEvent->FocusUnit->NotifyOutFocus(false);
			UIEvent->FocusUnit = nullptr;
		}

		return;
	}

	if (minimized)
		return;

	bool regularWindow;
	if ((UIEvent->FocusUnit && UIEvent->FocusUnit->GetWindow() == this) || !UIEvent->FocusUnit)
		regularWindow = true;
	else
		regularWindow = false; // if window in FocusUnit not regular

	// don't replase int because in "i" can be minus value 
	for (int i = (int)Viewports.size() - 1; i >= 0; i--)
	{
		FVector2D viewPos = Viewports[i]->GetPosition();
		float xEnd = (float)(viewPos.X + Viewports[i]->Width);
		float yEnd = (float)(viewPos.Y + Viewports[i]->Height);
		if (x >= viewPos.X && y >= viewPos.Y &&
			x <= xEnd && y <= yEnd)
		{
			UIEvent->NewFrame(&Viewports[i]->Root, (float)x, (float)y, QTime::GetDeltaTime(), RegularWindow);
			return;
		}
	}

	UIEvent->NewFrame(nullptr, (float)x, (float)y, QTime::GetDeltaTime(), RegularWindow);
}

void QFAEngineWindow::CheckIfNeedResizeWindows()
{
	for (size_t i = 0; i < Windows.size(); i++)
	{
		if (Windows[i]->WindowSizeChanched)
		{
			Windows[i]->recreateSwapChain();
			Windows[i]->Width = Windows[i]->NewWidth;
			Windows[i]->Height = Windows[i]->NewHeight;
			Windows[i]->WindowSizeChanched = false;
			for (int j = 0; j < Windows[i]->Viewports.size(); j++)
				Windows[i]->Viewports[j]->Settup(Windows[i]->Width, Windows[i]->Height);
		}
	}	
}

void QFAEngineWindow::StartFrame()
{
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

void QFAEngineWindow::PresentFrame(VkSemaphore finishSemi)
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

void QFAEngineWindow::CreateShadow()
{
	ShadowSampler = new QFAVKTextureSampler();

	ShadowImage= new QFAImage( QFAVKShadowFrameBuffer::GetShadowResolution(), QFAVKShadowFrameBuffer::GetShadowResolution(),4, QFAEngineWindow::depthFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	ShadowImagesView= new QFAVKImageView(ShadowImage, VK_IMAGE_ASPECT_DEPTH_BIT);
}



void QFAEngineWindow::DrawActors(QFAEngineViewport* _viewport, bool clear)
{
	CurentComandBuffer = ViewportStuff[ViewportProcess].comandBuffer;
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = clear ? frameBuffer->Clear.renderPass : frameBuffer->After.renderPass;
	renderPassInfo.framebuffer = frameBuffer->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->swapChainExtent;


	std::array<VkClearValue, 3> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
	clearValues[1].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
	clearValues[2].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(CurentComandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); 

	vkCmdBindPipeline(CurentComandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::Pipeline->GetPipeline());

	VkViewport viewport{};
	FVector2D viewPos = _viewport->GetPositionRelativeRegularWindow();
	viewport.x = viewPos.X;
	viewport.y = viewPos.Y;
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
		lightMat);
	unsigned int countComponentForRender = 0; 
	
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, false);

	vkCmdEndRenderPass(CurentComandBuffer);
}

unsigned int QFAEngineWindow::ProcessMeshComponent(QSceneComponent* component, bool shadow)
{
	if (!component->IsValid())
		return 0;

	unsigned int countComponentForRender = 0;
	if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
	{		
		if (mesh->Mf)
		{
			mesh->Render(CurentComandBuffer, shadow, CurentCameraPosition);
			countComponentForRender++;
		}
	}

	for (size_t i = 0; i < component->ListComponents.Length(); i++)
		countComponentForRender += ProcessMeshComponent(component->ListComponents[i], shadow);

	return countComponentForRender;
}


void QFAEngineWindow::CreateViewPortStuff()
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


void QFAEngineWindow::GetFocus()
{
	if (RegularWindow)
		glfwFocusWindow(glfWindow);
}

bool QFAEngineWindow::ShouldClose()
{
	if (RegularWindow)
		return glfwWindowShouldClose(glfWindow) || NeedClose;
	else
		return false;
}

void QFAEngineWindow::EnabelDecorated(bool enabel)
{
	if (RegularWindow)
		glfwSetWindowAttrib(glfWindow, GLFW_DECORATED, enabel);
}

void QFAEngineWindow::SetSize(int w, int h)
{
	if(RegularWindow)
		glfwSetWindowSize(glfWindow, w, h);	

	Width = w;
	Height = h;
}

void QFAEngineWindow::MoveToCenter()
{
	if(RegularWindow)
	{
		GLFWmonitor* pm = glfwGetPrimaryMonitor();
		int x, y, w, h;
		glfwGetMonitorWorkarea(pm, &x, &y, &w, &h);
		glfwSetWindowPos(glfWindow, w / 2 - Width / 2 + x, h / 2 - Height / 2 + y);
	}
}

QFAEngineWindow* QFAEngineWindow::GetMainWindows()
{
	if (Windows.size())
		return Windows[0];
	else
		return nullptr;
}