#include "Window.h"

#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>

#include <Render/Time.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Overlord/Overlord.h>
#include <Render/Window/Viewport.h>
#include <Render/Pipline/TextPipeline.h>
#include <chrono>
// vkDeviceWaitIdle(QFAVKLogicalDevice::GetDevice());
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/StaticMesh.h>
#include <Render/UI/Image.h>
#include <Render/Pipline/ImagePipeline.h>


#include <Render/Pipline/MeshShadowPipeline.h>
#include <Render/Framebuffer/MeshFrameBuffer.h> 

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

	QFAViewport* vp = new QFAViewport;
	Viewports.Add(vp);
	vp->Settup(width, height);

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
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	RenderPass = new QFAVKRenderPass(SwapChain->swapChainImageFormat, true);
	RenderPassOffScreen = new QFAVKRenderPassDepth();
	TextRenderPass = new QFAVKTextRenderPass(SwapChain->swapChainImageFormat);

	SwapChain->createFramebuffers(RenderPass->renderPass);
	
	frameBufferMesh = new QFAVKMeshFrameBuffer(commandPool, width, height);

	QFAText::Init(TextRenderPass->renderPass, commandPool);
	for (size_t i = 0; i < MaxActiveViewPort; i++)
		ShadowFrameBuffers[i] = new QFAVKShadowFrameBuffer(commandPool, RenderPassOffScreen->renderPass);

	CreateShadow();
	QMeshBaseComponent::Init(RenderPass->renderPass, RenderPassOffScreen->renderPass, commandPool);

	createCommandBuffer();
	createSyncObject();

	imugo = new QFAImage(commandPool);
	
	imugo->Init(RenderPass->renderPass, commandPool, frameBufferMesh->ColorImage);
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
	std::array<VkCommandBuffer, 3 * MaxActiveViewPort + 1> comb;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = comb.size();

	if (vkAllocateCommandBuffers(QFAVKLogicalDevice::GetDevice(), &allocInfo, comb.data()) != VK_SUCCESS)
		stopExecute("failed to allocate command buffers!");

	FinisCommandBuffer = comb.back();
	int index = 0;
	for (size_t i = 0; i < MaxActiveViewPort; i += 3)
	{
		ShadowCommandBuffers[index] = comb[i];
		MeshCommandBuffers[index] = comb[i + 1];
		UICommandBuffers[index] = comb[i + 2];
		index++;
	}
}

void QFAWindow::DrawText(QFAViewport* _viewport)
{

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->UICommandBuffers[ViewportProcess], &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");
		
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->TextRenderPass->renderPass;
	renderPassInfo.framebuffer = frameBufferMesh->Framebuffer;//SwapChain->swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = MainWindow->SwapChain->swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->UICommandBuffers[ViewportProcess], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);	

	VkViewport viewport{};
	viewport.x = _viewport->X;
	viewport.y = _viewport->Y;
	viewport.width = _viewport->Width;
	viewport.height = _viewport->Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->UICommandBuffers[ViewportProcess], 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = MainWindow->SwapChain->swapChainExtent;
	vkCmdSetScissor(MainWindow->UICommandBuffers[ViewportProcess], 0, 1, &scissor);


	vkCmdBindPipeline(MainWindow->UICommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS, QFAText::Pipeline->graphicsPipeline);
	QFAText::StartTextRender(_viewport->UIProjection);

	for (size_t j = 0; j < _viewport->Texts.Length(); j++)
	{
		_viewport->Texts[j]->Render();
		recordCommandBufferText(_viewport->Texts[j], _viewport);
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {   ActorFinishedSemaphore[ViewportProcess]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores ;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &UICommandBuffers[ViewportProcess];

	VkSemaphore signalSemaphores[] = { UISemaphore[ViewportProcess]};	
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	
	
	
	vkCmdEndRenderPass(UICommandBuffers[ViewportProcess]);

	if (vkEndCommandBuffer(UICommandBuffers[ViewportProcess]) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");	
}

void QFAWindow::DrawOffscreenBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->FinisCommandBuffer, &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");


	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->RenderPass->renderPass;
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
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = Width;
	viewport.height = Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->FinisCommandBuffer, 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = MainWindow->SwapChain->swapChainExtent;
	vkCmdSetScissor(MainWindow->FinisCommandBuffer, 0, 1, &scissor);

	vkCmdBindPipeline(MainWindow->FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, QFAImage::Pipeline->graphicsPipeline);

	recordCommandBufferTestImege();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	submitInfo.waitSemaphoreCount = ViewportProcess;
	submitInfo.pWaitSemaphores = &UISemaphore[0];// ;
	
	submitInfo.pWaitDstStageMask = UISemaphoreStages.data();

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &FinisCommandBuffer;

	VkSemaphore signalSemaphores[] = { FinisSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkCmdEndRenderPass(FinisCommandBuffer);

	
	QFAVKBuffer::transitionImageLayout(frameBufferMesh->ColorImage->TextureImage, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, commandPool, VK_IMAGE_ASPECT_COLOR_BIT);

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

	vkCmdBindDescriptorSets(FinisCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		QFAImage::Pipeline->pipelineLayout, 0, 1, &QFAVKImagePipeline::descriptorSet, 0, nullptr);
	
	vkCmdDraw(FinisCommandBuffer, static_cast<uint32_t>(6), 1, 0, 0);
}

void QFAWindow::createSyncObject()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;	
	
	if(vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &GetImageSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &FinisSemaphore) != VK_SUCCESS)
		stopExecute("failed to create Semaphore");

	for (size_t i = 0; i < QFAWindow::MaxActiveViewPort; i++)
	{
		if (vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &ActorFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &ActorShadowFinishedSemaphore[i]) != VK_SUCCESS ||			
			vkCreateSemaphore(QFAVKLogicalDevice::GetDevice(), &semaphoreInfo, nullptr, &UISemaphore[i]) != VK_SUCCESS )
			stopExecute("failed to create Semaphore");

		UISemaphoreStages[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

}

void QFAWindow::recreateSwapChain()
{
	delete SwapChain;
	SwapChain = new QFAVKSwapChain(glfWindow, surface, commandPool);
	SwapChain->createFramebuffers(RenderPass->renderPass);

	frameBufferMesh->ResizeBuffer(commandPool, NewWidth, NewHeight);
	delete imugo;
	imugo = new QFAImage(commandPool);
	imugo->Init(RenderPass->renderPass, commandPool, frameBufferMesh->ColorImage);
}

void QFAWindow::AddViewport(QFAViewport* viewport)
{
	Viewports.Add(viewport);
	viewport->Settup(Width, Height);
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



void QFAWindow::StartRenderOff()
{
	vkResetCommandBuffer(MainWindow->ShadowCommandBuffers[ViewportProcess], 0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(MainWindow->ShadowCommandBuffers[ViewportProcess], &beginInfo) != VK_SUCCESS)
		stopExecute("failed to begin recording command buffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = MainWindow->RenderPassOffScreen->renderPass;
	renderPassInfo.framebuffer = ShadowFrameBuffers[ViewportProcess]->Framebuffer;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = {(uint32_t)shadowResolution, (uint32_t)shadowResolution };

	std::array<VkClearValue, 1> clearValues{};	
	clearValues[0].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->ShadowCommandBuffers[ViewportProcess], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void QFAWindow::RenderOff(QFAViewport* _viewport)
{
	QMeshBaseComponent::StartShadowFrame();

	// Set depth bias (aka "Polygon offset")
// Required to avoid shadow mapping artifacts
	vkCmdSetDepthBias(
		MainWindow->ShadowCommandBuffers[ViewportProcess],
		depthBiasConstant,
		0.0f,
		depthBiasSlope);

	vkCmdBindPipeline(MainWindow->ShadowCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::ShadowPipline->graphicsPipeline);

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
	glm::mat4 liteMat = dl->GetLightMatrix(MainWindow->Viewports[QFAWindow::GetMainWindow()->ViewportProcess]->CurentCamera->GetWorldPosition().ConvertToVulkanCoordinate());
	
	QMeshBaseComponent::ShaderDL.ambient = dl->Ambient;
	QMeshBaseComponent::ShaderDL.diffuse = dl->Diffuse;
	QMeshBaseComponent::ShaderDL.direction = dl->Direction;
	QMeshBaseComponent::ShaderDL.specular = dl->Specular;
	
	QMeshBaseComponent::StartShadowFrameViewport(liteMat);
	unsigned int countComponentForRender = 0;
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, true);
}

void QFAWindow::EndRenderOff()
{
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
	
	vkCmdCopyImage(MainWindow->ShadowCommandBuffers[ViewportProcess], ShadowFrameBuffers[ViewportProcess]->depthImageQFA->TextureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ShadowImages[ViewportProcess]->TextureImage, VK_IMAGE_LAYOUT_GENERAL, 1, &ic);

	if (vkEndCommandBuffer(ShadowCommandBuffers[ViewportProcess]) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &ShadowCommandBuffers[ViewportProcess];

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &ActorShadowFinishedSemaphore[ViewportProcess];

	if (vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, nullptr/*inFlightFence*/) != VK_SUCCESS)
		stopExecute("failed to submit draw command buffer!");
}

void QFAWindow::RenderWindow()
{

	MainWindow->ViewportProcess = 0;

	MainWindow->StartFrame();

	for (size_t i = 0; i < MainWindow->Viewports.Length(); i++)
	{
		MainWindow->StartRenderOff();

		MainWindow->RenderOff(MainWindow->Viewports[i]); // in problem

		MainWindow->EndRenderOff();

		MainWindow->DrawActors(MainWindow->Viewports[i], i == 0);

		MainWindow->DrawText(MainWindow->Viewports[i]);

		MainWindow->ViewportProcess++;
	}

	MainWindow->DrawOffscreenBuffer();
	MainWindow->PresentFrame();
}

void QFAWindow::StartFrame()
{
	VkResult result = vkAcquireNextImageKHR(QFAVKLogicalDevice::GetDevice(), SwapChain->SwapChain, UINT64_MAX, GetImageSemaphore, nullptr, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();		
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		stopExecute("failed to acquire swap chain image!");

	if (WindowSizeChanched)
	{
		recreateSwapChain();


		Width = NewWidth;
		Height = NewHeight;
		WindowSizeChanched = false;
		for (int j = 0; j < Viewports.Length(); j++)
			Viewports[j]->Settup(Width, Height);
	}
}

void QFAWindow::PresentFrame()
{
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR; 
	
	VkSemaphore waitSemaphores[] = { GetImageSemaphore, FinisSemaphore};
	presentInfo.waitSemaphoreCount = 2;// ViewportProcess + 1;
	presentInfo.pWaitSemaphores = waitSemaphores;// FinisSemaphore.data();

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

	for (size_t i = 0; i < MaxActiveViewPort; i++)
	{		
		ShadowImages[i] = new QFAVKTextureImage(commandPool, QFAVKShadowFrameBuffer::GetShadowResolution(), QFAVKShadowFrameBuffer::GetShadowResolution(), 4, QFAWindow::depthFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
		ShadowImagesViews[i] = new QFAVKImageView(ShadowImages[i], VK_IMAGE_ASPECT_DEPTH_BIT);
	}
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
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(MainWindow->MeshCommandBuffers[ViewportProcess], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(MainWindow->MeshCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS, QMeshBaseComponent::Pipeline->graphicsPipeline);

	VkViewport viewport{};
	viewport.x = _viewport->X;
	viewport.y = _viewport->Y;
	viewport.width = _viewport->Width;
	viewport.height = _viewport->Height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(MainWindow->MeshCommandBuffers[ViewportProcess], 0, 1, &viewport); // can in start render viewport do 

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = MainWindow->SwapChain->swapChainExtent;
	vkCmdSetScissor(MainWindow->MeshCommandBuffers[ViewportProcess], 0, 1, &scissor);


	QWorld* world = _viewport->CurentCamera->GetWorld();
		
	glm::mat4 lightMat = world->GetDirectionDight()->GetLightMatrix(_viewport->CurentCamera->GetWorldPosition().ConvertToVulkanCoordinate());
	QMeshBaseComponent::StartFrameViewpoet(_viewport->MatrixPerspective,
		_viewport->CurentCamera->cameraRotationMatrex,
		_viewport->CurentCamera->WorldPosition,
		lightMat); 
	unsigned int countComponentForRender = 0; 
	
	
	for (size_t i = 0; i < world->Actors.Length(); i++)
		countComponentForRender += ProcessMeshComponent(world->Actors[i]->RootComponent, false);

	vkCmdEndRenderPass(MeshCommandBuffers[ViewportProcess]);

	if (vkEndCommandBuffer(MeshCommandBuffers[ViewportProcess]) != VK_SUCCESS)
		stopExecute("failed to record command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


	//VkSemaphore waitSemaphores[] = {  };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &ActorShadowFinishedSemaphore[ViewportProcess];
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &MeshCommandBuffers[ViewportProcess];


	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &ActorFinishedSemaphore[ViewportProcess];

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
		mesh->UpdateBuffers(0, shadow);
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
			QMeshBaseComponent::ShadowPipline->pipelineLayout, 0, 1, &nextSet, 0, nullptr);

		vkCmdDrawIndexed(ShadowCommandBuffers[ViewportProcess], static_cast<uint32_t>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}
	else
	{
		vkCmdBindVertexBuffers(MeshCommandBuffers[ViewportProcess], 0, 1, &mesh->VertexBufer->GpuSideBuffer->Buffer, offsets);
		vkCmdBindIndexBuffer(MeshCommandBuffers[ViewportProcess], mesh->IndexBuffer->GpuSideBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);

		auto nextSets = mesh->GetNextSets();
		vkCmdBindDescriptorSets(MeshCommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS,
			QMeshBaseComponent::Pipeline->pipelineLayout, 0, nextSets.size(), nextSets.data(), 0, nullptr);

		vkCmdDrawIndexed(MeshCommandBuffers[ViewportProcess], static_cast<uint32_t>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}	
}

void QFAWindow::recordCommandBufferText(QFAText* text, QFAViewport* viewPort)
{
	VkBuffer vertexBuffers[] = { text->vertexBufer->GpuSideBuffer->Buffer};
	VkDeviceSize offsets[] = { 0 };
	
	vkCmdBindVertexBuffers(UICommandBuffers[ViewportProcess], 0, 1, vertexBuffers, offsets);
	
	vkCmdBindDescriptorSets(UICommandBuffers[ViewportProcess], VK_PIPELINE_BIND_POINT_GRAPHICS,
		QFAText::Pipeline->pipelineLayout, 0, 1, &text->CurentDescriptorSet, 0, nullptr);
	
	vkCmdDraw(UICommandBuffers[ViewportProcess], static_cast<uint32_t>(text->CountSymbolForRender * 6), 1, 0, 0);
}


