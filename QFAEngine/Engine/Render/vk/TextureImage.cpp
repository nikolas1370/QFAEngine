#include "TextureImage.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#include <Render/vk/VKBuffer.h>
#include <Render/vk/PhysicalDevice.h>
#include <Render/vk/LogicalDevice.h>

QFAVKTextureImage::QFAVKTextureImage(VkCommandPool commandPool, int Width, int Height, unsigned int channelCount,  VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect )
{
    ImageFormat = format;
    VkDeviceSize imageSize = Width * Height * channelCount;
    buffer = new QFAVKBuffer(imageSize, nullptr);
    createImage(Width, Height, format, VK_IMAGE_TILING_OPTIMAL, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TextureImage, TextureImageMemory);
    

    
    buffer->copyInImage(this, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), commandPool,0,0 ,aspect, VK_IMAGE_LAYOUT_GENERAL);
}

QFAVKTextureImage::QFAVKTextureImage( VkCommandPool commandPool, const std::string src)
{    


    ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(src.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        stopExecute("failed to load texture image!");

    buffer = new QFAVKBuffer( imageSize, pixels);
    stbi_image_free(pixels);

    
    createImage(texWidth, texHeight, ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TextureImage, TextureImageMemory);
    buffer->copyInImage(this, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool);
}

QFAVKTextureImage::~QFAVKTextureImage()
{
    delete buffer;
    vkDestroyImage(QFAVKLogicalDevice::GetDevice(), TextureImage, nullptr);
    vkFreeMemory(QFAVKLogicalDevice::GetDevice(), TextureImageMemory, nullptr);
}

void QFAVKTextureImage::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{// findMemoryType
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

    if (vkCreateImage(QFAVKLogicalDevice::GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
        stopExecute("QFAVKTextureImage::createImage failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(QFAVKLogicalDevice::GetDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = QFAVKBuffer::findMemoryType( memRequirements.memoryTypeBits, properties);


    /*
 * limite 4096 allocation
 Memory suballocation
     https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/
 */
    if (vkAllocateMemory(QFAVKLogicalDevice::GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        stopExecute("QFAVKTextureImage::createImage failed to allocate image memory!");
    
    vkBindImageMemory(QFAVKLogicalDevice::GetDevice(), image, imageMemory, 0);
}