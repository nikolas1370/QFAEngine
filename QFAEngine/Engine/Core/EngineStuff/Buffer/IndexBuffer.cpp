#include "pch.h"
#include "IndexBuffer.h"
#include <Core/EngineStuff/vk/LogicalDevice.h>
#include <Core/EngineStuff/vk/PhysicalDevice.h>

QFAVKIndexBuffer::QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool)
{
	CpuSideBuffer = new QFAVKBuffer(size, data, true);
	GpuSideBuffer = new QFAVKBuffer(size, nullptr, false, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	CpuSideBuffer->copyBuffer(GpuSideBuffer->Buffer, size, commandPool);
}

QFAVKIndexBuffer::~QFAVKIndexBuffer()
{
	delete GpuSideBuffer;
	delete CpuSideBuffer;
}
