#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>

class QMeshBaseComponent;
class QFAEXPORT QFAVKIndexBuffer
{
	friend QFAWindow;
	friend QMeshBaseComponent;
	QFAVKBuffer* CpuSideBuffer;
	QFAVKBuffer* GpuSideBuffer;
public:
	QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKIndexBuffer();
};
