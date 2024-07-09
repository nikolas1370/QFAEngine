#pragma once
#include <Render/RenderPass/MeshRenderPass.h>
#include <Tools/VulkanSuff.h>
#include <Render/Image.h>
#include <Render/vk/LogicalDevice.h>
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