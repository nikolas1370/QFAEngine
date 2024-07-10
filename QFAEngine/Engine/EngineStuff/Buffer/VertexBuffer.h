#pragma once
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/Buffer/VKBuffer.h>

class QFAEngineWindow;
class QFAText;
class QFAUIImage;
class QFAUITextInput;
class QMeshBaseComponent;
class QFAEXPORT QFAVKVertexBuffer
{
	friend QFAEngineWindow;
	friend QFAText;
	friend QFAUIImage;
	friend QFAUITextInput;
	friend QMeshBaseComponent;
	QFAVKBuffer* CpuSideBuffer;
	QFAVKBuffer* GpuSideBuffer;
	VkCommandPool CommandPool;
public:
	QFAVKVertexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKVertexBuffer();	

	void UpdateData(unsigned int size, void* data);
};
