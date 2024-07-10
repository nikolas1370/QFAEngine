#include "pch.h"
#include "VertexBuffer.h"
#include <EngineStuff/vk/LogicalDevice.h>
#include <EngineStuff/vk/PhysicalDevice.h>

QFAVKVertexBuffer::QFAVKVertexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool)
{
	CommandPool = commandPool;
	CpuSideBuffer = new QFAVKBuffer(size , data, true);
	GpuSideBuffer = new QFAVKBuffer(size, nullptr, false, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	CpuSideBuffer->copyBuffer(GpuSideBuffer->Buffer, size, commandPool);
}

QFAVKVertexBuffer::~QFAVKVertexBuffer()
{
	delete CpuSideBuffer;
	delete GpuSideBuffer;
}

void QFAVKVertexBuffer::UpdateData(unsigned int size, void* data)
{
	CpuSideBuffer->UpdateData(size, data);
	CpuSideBuffer->copyBuffer(GpuSideBuffer->Buffer, size, CommandPool);
}
