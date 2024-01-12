#include "IndexBuffer.h"
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/PhysicalDevice.h>

QFAVKIndexBuffer::QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool)
{
	QFAVKBuffer tem(size, data);
	QFAVKBuffer::createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);
	tem.copyBuffer(IndexBuffer, size, commandPool);
}

QFAVKIndexBuffer::~QFAVKIndexBuffer()
{
	vkDestroyBuffer(QFAVKLogicalDevice::GetDevice(), IndexBuffer, nullptr);
	vkFreeMemory(QFAVKLogicalDevice::GetDevice(), IndexBufferMemory, nullptr);
}
