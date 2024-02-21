#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/Buffer/VKBuffer.h>

class QFAWindow;
class QFAText;
class QFAUIImage;
class QFAUITextInput;
class QFAVKVertexBuffer
{
	friend QFAWindow;
	friend QFAText;
	friend QFAUIImage;
	friend QFAUITextInput;
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
