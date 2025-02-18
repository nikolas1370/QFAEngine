#pragma once
#include "VKStruct.h"
#include <Core/EngineStuff/vk/PhysicalDevice.h>

class QFAVKLogicalDevice
{
	static bool IsInit;

	static VkDevice Device;
	static VkQueue GraphicsQueue;
	static VkQueue PresentQueue;
public:
	

	static void Init(VkSurfaceKHR surface);
	
	static inline VkDevice GetDevice()
	{
		return Device;
	}
	static inline VkQueue  GetGraphicsQueue()
	{
		return GraphicsQueue;
	}

	static inline VkQueue  GetPresentQueue()
	{
		return PresentQueue;
	}
private:
	

};
