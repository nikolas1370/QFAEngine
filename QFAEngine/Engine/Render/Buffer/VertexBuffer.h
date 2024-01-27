#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>

class QFAWindow;
class QFAText;
class QFAUIImage;
class QFAVKVertexBuffer
{
	friend QFAWindow;
	friend QFAText;
	friend QFAUIImage;
	QFAVKBuffer* CpuSideBuffer;
	QFAVKBuffer* GpuSideBuffer;
	VkCommandPool CommandPool;
public:
	//VkBuffer VertexBuffer;
	//VkDeviceMemory VertexBufferMemory; 
	QFAVKVertexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKVertexBuffer();	

	void UpdateData(unsigned int size, void* data);

private:

};
