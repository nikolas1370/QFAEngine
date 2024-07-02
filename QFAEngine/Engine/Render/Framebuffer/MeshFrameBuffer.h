#pragma once
#include <Render/RenderPass/MeshRenderPass.h>
#include <Tools/VulkanSuff.h>
#include <Render/Image.h>
#include <Render/vk/LogicalDevice.h>
class QFAEXPORT QFAVKMeshFrameBuffer
{
	
	void CreateBuffer(VkCommandPool commandPool, int w, int h);

public:
	QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h, VkFormat imageFormat);
	~QFAVKMeshFrameBuffer();
	QFAImage* ColorImage;
	QFAImage* IdImage;

	VkImageView ColorImageView;
	VkImageView IdImageView;


	QFAImage* DepthImage;
	VkImageView DepthImageView;


	QFAVKMeshRenderPass Clear;
	QFAVKMeshRenderPass After;


	VkFramebuffer Framebuffer;

	void ResizeBuffer(VkCommandPool commandPool, int w, int h);
	
};