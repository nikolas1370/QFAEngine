#include "pch.h"
#include "Image.h"
#include <EngineStuff/Buffer/VKBuffer.h>
#include <EngineStuff/vk/PhysicalDevice.h>
#include <EngineStuff/vk/LogicalDevice.h>
#include <Tools/File/FileSystem.h>
#include <EngineStuff/Window/Window.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

QFAImage::QFAImage(SImageCreateInfo& ici)
{
    ImageFormat = ici.format;
    VkDeviceSize imageSize = ici.Width * ici.Height * ici.channelCount;
    if(ici.createBuffer)
        buffer = new QFAVKBuffer(imageSize, nullptr, true);

    CreateImageVK(ici.Width, ici.Height, ici.format, VK_IMAGE_TILING_OPTIMAL, ici.usage);
    QFAVKBuffer::transitionImageLayout(TextureImage, ici.format, VK_IMAGE_LAYOUT_UNDEFINED, ici.layout, QFAEngineWindow::QFAEngineWindow::commandPool, ici.aspect);
    ImageView.CreateView(this, ici.aspect);
    Width = ici.Width;
    Height = ici.Height;
}

QFAImage::QFAImage( int width, int height, unsigned int channelCount,  VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect , VkImageCreateFlags flags)
{
    ImageFormat = format;
    VkDeviceSize imageSize = width * height * channelCount;
    buffer = new QFAVKBuffer(imageSize, nullptr, true);
    CreateImageVK(width, height, format, VK_IMAGE_TILING_OPTIMAL, usage, flags);
    QFAVKBuffer::transitionImageLayout(TextureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, QFAEngineWindow::QFAEngineWindow::commandPool, aspect);
    ImageView.CreateView(this, aspect);
    Width = width;
    Height = height;
}

QFAImage::QFAImage(const std::u32string src)
{
    ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    int texWidth, texHeight, texChannels;
    // stbi_load_16  stbi_load_from_memory
    QFAFile file;
    QFAFileSystem::LoadFile(src, &file);
    stbi_uc* pixels = stbi_load_from_memory((const unsigned char*)file.GetData(), (int)file.GetDataSize(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels)
        stopExecute("failed to load texture image!");

    buffer = new QFAVKBuffer(imageSize, pixels, true);
    stbi_image_free(pixels);

    CreateImageVK(texWidth, texHeight, ImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    buffer->copyInImage(this, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), QFAEngineWindow::commandPool);

    ImageView.CreateView(this, VK_IMAGE_ASPECT_COLOR_BIT);
    Width = texWidth;
    Height = texHeight;
}

void QFAImage::SetImage(void* pixels)
{
    if (buffer)
        return;

    buffer = new QFAVKBuffer(Width * Height * 4, pixels, true);
    buffer->copyInImage(this, static_cast<uint32_t>(Width), static_cast<uint32_t>(Height), QFAEngineWindow::commandPool);
}


QFAImage::~QFAImage()
{
    if(TextureImage)
        vmaDestroyImage(QFAVKBuffer::allocator, TextureImage, ImageAllocation);

    if (buffer)
        delete buffer;
}

#if QFA_EDITOR_ONLY
    void QFAImage::UpdateImage(QFAImage* image)
    {
        if (!image)
            return;
     
        for (size_t i = 0; i < Images.size(); i++)
        {
            Images[i]->Image = nullptr;
            Images[i]->SetImage(image);
        }

        delete this;
    }

    void QFAImage::DeleteMeFromList(QFAUIImage* image)
    {
        for (size_t i = 0; i < Images.size(); i++)
        {
            if (Images[i] == image)
            {
                Images.erase(Images.begin() + i);
                return;
            }
        }
    }
#endif

void QFAImage::DeleteImageInCpuSide()
{
    if (buffer)
    {
        delete buffer;
        buffer = nullptr;
    }
}

void QFAImage::CreateImageVK(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags       flags)
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
    imageInfo.flags = flags;
    
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    VkResult res = vmaCreateImage(QFAVKBuffer::allocator,
        &imageInfo,
        &allocInfo,
        &TextureImage,
        &ImageAllocation,
        nullptr);
}
