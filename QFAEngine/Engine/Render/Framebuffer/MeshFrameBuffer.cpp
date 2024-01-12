#include "MeshFrameBuffer.h"

QFAVKMeshFrameBuffer::QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h)
{
	Clear.CreateRenderPass(true);
	After.CreateRenderPass(false);
	CreateBuffer(commandPool, w, h);
}

QFAVKMeshFrameBuffer::~QFAVKMeshFrameBuffer()
{

}

void QFAVKMeshFrameBuffer::ResizeBuffer(VkCommandPool commandPool, int w, int h)
{
	delete ColorImage;
	delete DepthImage;
	CreateBuffer(commandPool, w, h);
}

void QFAVKMeshFrameBuffer::CreateBuffer(VkCommandPool commandPool, int w, int h)
{

	ColorImage = new QFAVKTextureImage(commandPool, w, h, 4, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = ColorImage->TextureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_B8G8R8A8_SRGB;

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo, nullptr, &ColorImageView) != VK_SUCCESS)
		stopExecute("failed to create texture image view!");

	DepthImage = new QFAVKTextureImage(commandPool, w, h, 4, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	VkImageViewCreateInfo viewInfo2{};
	viewInfo2.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo2.image = DepthImage->TextureImage;
	viewInfo2.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo2.format = VK_FORMAT_D32_SFLOAT;

	viewInfo2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo2.subresourceRange.baseMipLevel = 0;
	viewInfo2.subresourceRange.levelCount = 1;
	viewInfo2.subresourceRange.baseArrayLayer = 0;
	viewInfo2.subresourceRange.layerCount = 1;

	if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo2, nullptr, &DepthImageView) != VK_SUCCESS)
		stopExecute("failed to create texture image view!");


	std::array<VkImageView, 2> attachments =
	{
		ColorImageView,
		DepthImageView
	};

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = Clear.renderPass;
	framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
	framebufferInfo.pAttachments = attachments.data();
	framebufferInfo.width = w;
	framebufferInfo.height = h;
	framebufferInfo.layers = 1;
	framebufferInfo.flags = 0;

	if (vkCreateFramebuffer(QFAVKLogicalDevice::GetDevice(), &framebufferInfo, nullptr, &Framebuffer) != VK_SUCCESS)
		stopExecute("failed to create framebuffer!");
}
