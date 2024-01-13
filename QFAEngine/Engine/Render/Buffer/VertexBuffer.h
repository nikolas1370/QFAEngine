#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Buffer/VKBuffer.h>

class QFAWindow;
class QFAVKVertexBuffer
{
	friend QFAWindow;
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
