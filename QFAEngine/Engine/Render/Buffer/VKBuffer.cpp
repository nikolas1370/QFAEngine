#include "VKBuffer.h"
#include <cstring>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/PhysicalDevice.h>
#include <Render/Image.h>
#include <Overlord/Overlord.h>

VmaAllocator QFAVKBuffer::allocator = nullptr;


std::vector<QFAVKBuffer::BufferTaskOtherThread> QFAVKBuffer::Tasks;
std::vector<QFAVKBuffer::SNotNeedBuffer> QFAVKBuffer::NotNeedBuffers;

void QFAVKBuffer::Init(VkInstance instance)
{
    if (allocator)
        return;
    
    VmaVulkanFunctions vulkanFunctions = {};
    vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
    vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;
    


    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorCreateInfo.physicalDevice = QFAVKPhysicalDevice::GetDevice();
    allocatorCreateInfo.device = QFAVKLogicalDevice::GetDevice();
    allocatorCreateInfo.instance = instance;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;


    vmaCreateAllocator(&allocatorCreateInfo, &allocator);
}

QFAVKBuffer::QFAVKBuffer(VkDeviceSize size, const void* data, bool inHost, VkBufferUsageFlags usage)
{
    InHost = inHost;
    CreateBufferInside(size, usage);    
    if (data == nullptr || !inHost)
        return;
    
    memcpy(MapData, data, static_cast<size_t>(size));
}

void QFAVKBuffer::CreateBufferInside(VkDeviceSize size, VkBufferUsageFlags usage)
{// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/usage_patterns.html
    
    VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferInfo.size = size;
    bufferInfo.usage = usage;    
    bufferInfo.pNext = nullptr;
    //
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = InHost ? VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
                             : VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

    if(VkResult res = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &Buffer, &allocation, nullptr))
       stopExecute(res)    

    if(InHost)
        vmaMapMemory(allocator, allocation, &MapData);
}

QFAVKBuffer::~QFAVKBuffer()
{
    SNotNeedBuffer nnb;
    nnb.isHost = InHost;
    nnb.buffer = Buffer;
    nnb.allocation = allocation;
    NotNeedBuffers.push_back(nnb);
}

uint32_t QFAVKBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(QFAVKPhysicalDevice::GetDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
            return i;

    stopExecute("QFAVKBuffer::findMemoryType failed to find suitable memory type!");
}

void QFAVKBuffer::UpdateData(unsigned int size, void* data)
{
    memcpy(MapData, data, static_cast<size_t>(size));   
}

void QFAVKBuffer::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkImageAspectFlags aspect)
{
    if (QFAOverlord::GetMainThreadId() != std::this_thread::get_id())
    {
        BufferTaskOtherThread btot;
        btot.type = ETaskType::TTtransition;
        btot.vKImage = image;
        btot.format = format;
        btot.oldLayout = oldLayout;
        btot.newLayout = newLayout;
        btot.commandPool = commandPool;
        btot.aspect = aspect;
        Tasks.push_back(btot);
        return;
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.pNext = nullptr;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    /*
    
    don't know what do do . need more ifo
        
    
    */
    else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        
        
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    ///
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        //  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL  VK_IMAGE_LAYOUT_GENERAL
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }


    else if (oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    // 
    
    else 
        stopExecute("unsupported layout transition!");

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandPool, commandBuffer);
}

void QFAVKBuffer::EndLife()
{
    vmaDestroyAllocator(allocator);
}


void QFAVKBuffer::ProcessTaskFromOtherThread()
{
    for (size_t i = 0; i < Tasks.size(); i++)
    {
        if (Tasks[i].type == ETaskType::TTBuffer)
            Tasks[i].buffer->copyBuffer(Tasks[i].dstBuffer, Tasks[i].size, Tasks[i].commandPool, Tasks[i].srcOffset, Tasks[i].dstOffset);
        else if (Tasks[i].type == ETaskType::TTImage)
            Tasks[i].buffer->copyInImage(Tasks[i].image, Tasks[i].width, Tasks[i].height, Tasks[i].commandPool, Tasks[i].imageOffsetX, Tasks[i].imageOffsetY, Tasks[i].aspect, Tasks[i].endLayout);
        else if (Tasks[i].type == ETaskType::TTtransition)
            transitionImageLayout(Tasks[i].vKImage, Tasks[i].format, Tasks[i].oldLayout, Tasks[i].newLayout, Tasks[i].commandPool, Tasks[i].aspect);
    }

    Tasks.clear();
}

void QFAVKBuffer::DeleteNotNeedBuffer()
{
    for (size_t i = 0; i < NotNeedBuffers.size(); i++)
    {
        if (NotNeedBuffers[i].isHost)
            vmaUnmapMemory(allocator, NotNeedBuffers[i].allocation);

        vmaDestroyBuffer(allocator, NotNeedBuffers[i].buffer, NotNeedBuffers[i].allocation);
    }

    NotNeedBuffers.clear();
}

void QFAVKBuffer::copyInImage(QFAImage* image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX, int32_t imageOffsetY, VkImageAspectFlags aspect, VkImageLayout endLayout)
{
    if (QFAOverlord::GetMainThreadId() != std::this_thread::get_id())
    {
        BufferTaskOtherThread btot;
        btot.type = ETaskType::TTImage;
        btot.buffer = this;
        btot.image = image;
        btot.width = width;
        btot.height = height;
        btot.commandPool = commandPool;
        btot.imageOffsetX = imageOffsetX;
        btot.imageOffsetY = imageOffsetY;
        btot.aspect = aspect;
        btot.endLayout = endLayout;
        Tasks.push_back(btot);
        return;
    }

    transitionImageLayout(image->TextureImage, image->ImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool, aspect);
    copyBufferToImage(image->TextureImage, width, height, commandPool, imageOffsetX, imageOffsetY, aspect);
    if (endLayout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        transitionImageLayout(image->TextureImage, image->ImageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, endLayout, commandPool, aspect);
}

void QFAVKBuffer::copyBuffer(VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
    if (QFAOverlord::GetMainThreadId() != std::this_thread::get_id())
    {
        BufferTaskOtherThread btot;
        btot.type = ETaskType::TTBuffer;
        btot.buffer = this;
        btot.dstBuffer = dstBuffer;
        btot.size = size;
        btot.commandPool = commandPool;
        btot.srcOffset = srcOffset;
        btot.dstOffset = dstOffset;
        Tasks.push_back(btot);
        return;
    }

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    vkCmdCopyBuffer(commandBuffer, Buffer, dstBuffer, 1, &copyRegion);//does not copy more than allocated memory in dstBuffer


    endSingleTimeCommands(commandPool, commandBuffer);
}



void QFAVKBuffer::copyBufferToImage(VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX, int32_t imageOffsetY, VkImageAspectFlags aspect )
{
    //  imageOffsetX, int32_t imageOffsetY,
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = aspect;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { imageOffsetX, imageOffsetY, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, Buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandPool, commandBuffer);
    
}




VkCommandBuffer QFAVKBuffer::beginSingleTimeCommands(VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    allocInfo.pNext = nullptr;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(QFAVKLogicalDevice::GetDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pNext = nullptr;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void QFAVKBuffer::endSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pNext = nullptr;

    vkQueueSubmit(QFAVKLogicalDevice::GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(QFAVKLogicalDevice::GetGraphicsQueue());

    vkFreeCommandBuffers(QFAVKLogicalDevice::GetDevice(), commandPool, 1, &commandBuffer);
}