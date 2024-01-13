#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Buffer/VKBuffer.h>

class QFAWindow;
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
