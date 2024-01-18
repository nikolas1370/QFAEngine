#pragma once
#include <Render/RenderPass/MeshRenderPass.h>
#include <Tools/VulkanSuff.h>
#include <Render/vk/TextureImage.h>
#include <Render/vk/LogicalDevice.h>
#include <Render/Pipline/MeshPipeline.h>
class QFAVKMeshFrameBuffer
{
	
	void CreateBuffer(VkCommandPool commandPool, int w, int h);

public:
	QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h);
	~QFAVKMeshFrameBuffer();
	QFAVKTextureImage* ColorImage;
	VkImageView ColorImageView;


	QFAVKTextureImage* DepthImage;
	VkImageView DepthImageView;


	QFAVKMeshRenderPass Clear;
	QFAVKMeshRenderPass After;


	VkFramebuffer Framebuffer;

	void ResizeBuffer(VkCommandPool commandPool, int w, int h);
	
};