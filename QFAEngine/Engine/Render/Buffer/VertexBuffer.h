#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Buffer/VKBuffer.h>
class QFAVKVertexBuffer
{

	QFAVKBuffer* buf;
	VkCommandPool CommandPool;
public:
	VkBuffer VertexBuffer;
	VkDeviceMemory VertexBufferMemory; 
	QFAVKVertexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKVertexBuffer();	

	void UpdateData(unsigned int size, void* data);
	
private:

};
