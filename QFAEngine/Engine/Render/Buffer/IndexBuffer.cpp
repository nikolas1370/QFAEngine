#include "IndexBuffer.h"
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/PhysicalDevice.h>

QFAVKIndexBuffer::QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool)
{
	QFAVKBuffer tem(size, data, true);
	GpuSideBuffer = new QFAVKBuffer(size, nullptr, false, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	tem.copyBuffer(GpuSideBuffer->Buffer, size, commandPool);
}

QFAVKIndexBuffer::~QFAVKIndexBuffer()
{
	delete GpuSideBuffer;
}
