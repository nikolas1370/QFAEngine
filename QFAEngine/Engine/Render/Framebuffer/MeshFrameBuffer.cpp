#include "MeshFrameBuffer.h"

QFAVKMeshFrameBuffer::QFAVKMeshFrameBuffer(VkCommandPool commandPool, int w, int h, VkFormat imageFormat)
{
	Clear.CreateRenderPass(true, imageFormat);
	After.CreateRenderPass(false, imageFormat);
	
	CreateBuffer(commandPool, w, h);

	
}

QFAVKMeshFrameBuffer::~QFAVKMeshFrameBuffer()
{

}

void QFAVKMeshFrameBuffer::ResizeBuffer(VkCommandPool commandPool, int w, int h)
{
	delete ColorImage;
	delete IdImage;
	delete DepthImage;
	CreateBuffer(commandPool, w, h);
}

void QFAVKMeshFrameBuffer::CreateBuffer(VkCommandPool commandPool, int w, int h)
{ 


	QFAImage::SImageCreateInfo ifo;
	ifo.channelCount = 4;
	ifo.format = VK_FORMAT_R8G8B8A8_SRGB;
	ifo.Height = h;
	ifo.Width = w;
	ifo.layout = VK_IMAGE_LAYOUT_GENERAL;
	ifo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	ColorImage = new QFAImage(ifo);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = ColorImage->TextureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	

	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; 
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	
	if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo, nullptr, &ColorImageView) != VK_SUCCESS)
		stopExecute("failed to create texture image view!");
	/*-----*/
	
	ifo.format = VK_FORMAT_R32_UINT;
	IdImage = new QFAImage(ifo);
	
	viewInfo.image = IdImage->TextureImage;
	viewInfo.format = VK_FORMAT_R32_UINT;
	if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo, nullptr, &IdImageView) != VK_SUCCESS)
		stopExecute("failed to create texture image view!");
	
	/*-----*/
	DepthImage = new QFAImage(w, h, 4, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

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

	
	
	std::array<VkImageView, 3> attachments =
	{
		ColorImageView,
		IdImageView,
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
	
	

	if (vkCreateFramebuffer(QFAVKLogicalDevice::GetDevice(), &framebufferInfo, nullptr, &Framebuffer) != VK_SUCCESS)
		stopExecute("failed to create framebuffer!");
	

}
