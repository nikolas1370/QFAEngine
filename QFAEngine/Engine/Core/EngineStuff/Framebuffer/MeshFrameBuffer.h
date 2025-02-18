#pragma once
#include <Core/EngineStuff/RenderPass/MeshRenderPass.h>
#include <Core/EngineStuff/VulkanSuff.h>
#include <Core/EngineStuff/Image.h>
#include <Core/EngineStuff/vk/LogicalDevice.h>
class QFAEXPORT QFAVKMeshFrameBuffer
{
public:
	QFAImage* ColorImage;
	QFAImage* IdImage;
	VkImageView ColorImageView;
	VkImageView IdImageView;
	QFAImage* DepthImage;
	VkImageView DepthImageView;
	QFAVKMeshRenderPass Clear;
	QFAVKMeshRenderPass After;
	VkFramebuffer Framebuffer;

private:

	void CreateBuffer(VkCommandPool commandPool, int w, int h);

public:
	QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h, VkFormat imageFormat);
	~QFAVKMeshFrameBuffer();
	void ResizeBuffer(VkCommandPool commandPool, int w, int h);
	
};