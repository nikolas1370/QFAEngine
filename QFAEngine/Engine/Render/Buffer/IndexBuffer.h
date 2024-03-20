#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>

class QMeshBaseComponent;
class QFAVKIndexBuffer
{
	friend QFAWindow;
	friend QMeshBaseComponent;
	QFAVKBuffer* CpuSideBuffer;
	QFAVKBuffer* GpuSideBuffer;
public:
	//VkBuffer IndexBuffer;
	
	QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKIndexBuffer();

private:

};
