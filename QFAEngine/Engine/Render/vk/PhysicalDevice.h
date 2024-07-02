#pragma once
#include <vulkan/vulkan.h>
#include <Render/vk/VKInstance.h>
#include "VKStruct.h"

class QFAVKPhysicalDevice
{

	static bool IsInit;

	static VkPhysicalDevice PhysicalDevice;	
public:
	static const std::vector<const char*> deviceExtensions;
	static void Init(QFAVKInstance* instance, VkSurfaceKHR surface);
	static inline VkPhysicalDevice GetDevice()
	{
		return PhysicalDevice;
	}
private:
	static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	static bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	static QFAVKStruct::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

};

	
