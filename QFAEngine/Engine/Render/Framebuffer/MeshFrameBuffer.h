#pragma once
#include <Render/RenderPass/MeshRenderPass.h>
#include <Tools/VulkanSuff.h>
#include <Render/Image.h>
#include <Render/vk/LogicalDevice.h>
#include <Render/Pipline/MeshPipeline.h>
class QFAVKMeshFrameBuffer
{
	
	void CreateBuffer(VkCommandPool commandPool, int w, int h);

public:
	QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h);
	~QFAVKMeshFrameBuffer();
	QFAImage* ColorImage;
	VkImageView ColorImageView;


	QFAImage* DepthImage;
	VkImageView DepthImageView;


	QFAVKMeshRenderPass Clear;
	QFAVKMeshRenderPass After;


	VkFramebuffer Framebuffer;

	void ResizeBuffer(VkCommandPool commandPool, int w, int h);
	
};