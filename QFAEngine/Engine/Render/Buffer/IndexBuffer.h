#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>


class QFAVKIndexBuffer
{
	friend QFAWindow;
	QFAVKBuffer* GpuSideBuffer;
public:
	//VkBuffer IndexBuffer;
	
	QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKIndexBuffer();

private:

};
