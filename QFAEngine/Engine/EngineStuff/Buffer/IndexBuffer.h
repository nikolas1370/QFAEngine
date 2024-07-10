#pragma once
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/Buffer/VKBuffer.h>

class QMeshBaseComponent;
class QFAEXPORT QFAVKIndexBuffer
{
	friend QFAEngineWindow;
	friend QMeshBaseComponent;
	QFAVKBuffer* CpuSideBuffer;
	QFAVKBuffer* GpuSideBuffer;
public:
	QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKIndexBuffer();
};
