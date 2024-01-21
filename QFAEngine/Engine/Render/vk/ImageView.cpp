#include "ImageView.h"
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/TextureImage.h>


QFAVKImageView::QFAVKImageView(QFAVKTextureImage* ti, VkImageAspectFlags as)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = ti->TextureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = ti->ImageFormat;
    viewInfo.subresourceRange.aspectMask = as;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo, nullptr, &ImageView) != VK_SUCCESS)
        stopExecute("failed to create texture image view!");

    
}

QFAVKImageView::~QFAVKImageView()
{
    vkDestroyImageView(QFAVKLogicalDevice::GetDevice(), ImageView, nullptr);
}


