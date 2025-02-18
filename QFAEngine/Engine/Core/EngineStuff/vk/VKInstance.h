#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>


class QFAVKInstance
{


#ifdef _DEBUG
	VkDebugUtilsMessengerEXT debugMessenger;
#endif
public:
	VkInstance instance;

#ifdef _DEBUG

	static const std::vector<const char*> validationLayers;
private:

	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << "\n\n";

		return VK_FALSE;
	}

	void setupDebugMessenger();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
#endif
private:
	std::vector<const char*> getRequiredExtensions();

public:
	QFAVKInstance();
};



