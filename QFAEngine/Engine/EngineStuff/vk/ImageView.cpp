#include "pch.h"
#include "ImageView.h"
#include <EngineStuff/vk/LogicalDevice.h>
#include <EngineStuff/Image.h>


void QFAVKImageView::CreateView(QFAImage* ti, VkImageAspectFlags as)
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


QFAVKImageView::QFAVKImageView()
{
}

QFAVKImageView::QFAVKImageView(QFAImage* ti, VkImageAspectFlags as)
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
    if(ImageView)
        vkDestroyImageView(QFAVKLogicalDevice::GetDevice(), ImageView, nullptr);
}


void QFAVKImageView::UpdateView(QFAVKImageView* view)
{
    if (ImageView)
        vkDestroyImageView(QFAVKLogicalDevice::GetDevice(), ImageView, nullptr);

    ImageView = view->ImageView;

    view->ImageView = nullptr;// not need delete view, QFAVKImageView not create with new 
}
