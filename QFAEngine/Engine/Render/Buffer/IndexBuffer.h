#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Buffer/VKBuffer.h>
class QFAVKIndexBuffer
{

public:
	VkBuffer IndexBuffer;
	VkDeviceMemory IndexBufferMemory;
	QFAVKIndexBuffer(VkDeviceSize size, const void* data, VkCommandPool commandPool);
	~QFAVKIndexBuffer();

private:

};
