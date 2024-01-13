#include "TextureImage.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include <Render/Buffer/VKBuffer.h>
#include <Render/vk/PhysicalDevice.h>
#include <Render/vk/LogicalDevice.h>

QFAVKTextureImage::QFAVKTextureImage(VkCommandPool commandPool, int Width, int Height, unsigned int channelCount,  VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect )
{
    ImageFormat = format;
    VkDeviceSize imageSize = Width * Height * channelCount;
    buffer = new QFAVKBuffer(imageSize, nullptr, true);
    createImage(Width, Height, format, VK_IMAGE_TILING_OPTIMAL, usage);    
    QFAVKBuffer::transitionImageLayout(TextureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, commandPool, aspect);
}

QFAVKTextureImage::QFAVKTextureImage( VkCommandPool commandPool, const std::string src)
{   
    ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(src.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        stopExecute("failed to load texture image!");

    buffer = new QFAVKBuffer( imageSize, pixels, true);
    stbi_image_free(pixels);
    
    createImage(texWidth, texHeight, ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    buffer->copyInImage(this, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool);
}

QFAVKTextureImage::~QFAVKTextureImage()
{
    vmaDestroyImage(QFAVKBuffer::allocator, TextureImage, ImageAllocation);
    delete buffer;
}

void QFAVKTextureImage::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;


    VmaAllocationInfo AllocationInfo;
    VkResult res = vmaCreateImage(QFAVKBuffer::allocator,
        &imageInfo,
        &allocInfo,
        &TextureImage,
        &ImageAllocation,
        nullptr);
}