#include "PhysicalDevice.h"
#include <Tools/VulkanSuff.h>
#include <set>
#include <Render/vk/QueueFamilies.h>

const std::vector<const char*> QFAVKPhysicalDevice::deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME };
VkPhysicalDevice QFAVKPhysicalDevice::PhysicalDevice = VK_NULL_HANDLE;
bool QFAVKPhysicalDevice::IsInit = false;

void QFAVKPhysicalDevice::Init(QFAVKInstance* instance, VkSurfaceKHR surface)
{
    if (IsInit)
        return;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, nullptr);

    if (deviceCount == 0)
        stopExecute("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance->instance, &deviceCount, devices.data());


    /*
    * 
    * 
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    * 
        VkPhysicalDeviceVulkan12Features as; // runtimeDescriptorArray
    as.runtimeDescriptorArray = true;
    vkGetPhysicalDeviceFeatures2()
    */
    
        

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device, surface))
        {
            PhysicalDevice = device;
            break;
        }
    }

    if (PhysicalDevice == VK_NULL_HANDLE)
        stopExecute("failed to find a suitable GPU!");

    IsInit = true;
}

bool QFAVKPhysicalDevice::isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QFAVKStruct::QueueFamilyIndices indices = QFAVKQueueFamilies::findQueueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) 
    {
        QFAVKStruct::SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}



bool QFAVKPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) 
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}


QFAVKStruct::SwapChainSupportDetails QFAVKPhysicalDevice::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QFAVKStruct::SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
