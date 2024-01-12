#include "VertexBuffer.h"

#include <Render/vk/LogicalDevice.h>
#include <Render/vk/PhysicalDevice.h>

QFAVKVertexBuffer::QFAVKVertexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool)
{
	CommandPool = commandPool;
	buf = new QFAVKBuffer(size , data);
	QFAVKBuffer::createBuffer(size , VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);
	buf->copyBuffer(VertexBuffer, size, commandPool);
}

QFAVKVertexBuffer::~QFAVKVertexBuffer()
{
	delete buf;
	vkDestroyBuffer(QFAVKLogicalDevice::GetDevice(), VertexBuffer, nullptr);
	vkFreeMemory(QFAVKLogicalDevice::GetDevice(), VertexBufferMemory, nullptr);
}

void QFAVKVertexBuffer::UpdateData(unsigned int size, void* data)
{
	buf->UpdateData(size, data);
	buf->copyBuffer(VertexBuffer, size, CommandPool);
}
