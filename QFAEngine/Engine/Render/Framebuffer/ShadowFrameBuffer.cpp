#include "pch.h"
#include "ShadowFrameBuffer.h"
#include <Render/vk/LogicalDevice.h>
#include <Tools/Stuff.h>

QFAVKShadowFrameBuffer::QFAVKShadowFrameBuffer(VkCommandPool commandPool, VkRenderPass renderPass)
{
	depthImageQFA = new QFAImage( shadowResolution, shadowResolution, 4, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	VkImageViewCreateInfo viewInfo2{};
	viewInfo2.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo2.image = depthImageQFA->TextureImage;
	viewInfo2.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo2.format = VK_FORMAT_D32_SFLOAT;//format;

	viewInfo2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	viewInfo2.subresourceRange.baseMipLevel = 0;
	viewInfo2.subresourceRange.levelCount = 1;
	viewInfo2.subresourceRange.baseArrayLayer = 0;
	viewInfo2.subresourceRange.layerCount = 1;


	if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo2, nullptr, &depthImageView) != VK_SUCCESS)
		stopExecute("failed to create texture image view!");

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &depthImageView;
	framebufferInfo.width = shadowResolution;
	framebufferInfo.height = shadowResolution;
	framebufferInfo.layers = 1;
	framebufferInfo.flags = 0;

	if (vkCreateFramebuffer(QFAVKLogicalDevice::GetDevice(), &framebufferInfo, nullptr, &Framebuffer) != VK_SUCCESS)
		stopExecute("failed to create framebuffer!");

}

QFAVKShadowFrameBuffer::~QFAVKShadowFrameBuffer()
{
}




