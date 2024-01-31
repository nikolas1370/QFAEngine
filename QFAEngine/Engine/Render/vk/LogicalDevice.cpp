#include "LogicalDevice.h"
#include <set>
#include <Tools/VulkanSuff.h>
#include <Render/vk/QueueFamilies.h>

bool QFAVKLogicalDevice::IsInit = false;
VkDevice QFAVKLogicalDevice::Device;
VkQueue QFAVKLogicalDevice::GraphicsQueue;
VkQueue QFAVKLogicalDevice::PresentQueue;

void QFAVKLogicalDevice::Init( VkSurfaceKHR surface)
{
    if (IsInit)
        return;

    QFAVKStruct::QueueFamilyIndices indices = QFAVKQueueFamilies::findQueueFamilies(QFAVKPhysicalDevice::GetDevice(), surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };



    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.independentBlend = true;
    
    VkPhysicalDeviceVulkan12Features features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features.pNext = nullptr;
    features.runtimeDescriptorArray = VK_TRUE; // enable GL_EXT_nonuniform_qualifier
    features.descriptorIndexing = VK_TRUE;
    //features.descriptorBindingPartiallyBound = VK_TRUE;
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.pNext = &features;


    createInfo.enabledExtensionCount = static_cast<uint32_t>(QFAVKPhysicalDevice::deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = QFAVKPhysicalDevice::deviceExtensions.data();

    
#ifdef _DEBUG    
    createInfo.enabledLayerCount = static_cast<uint32_t>(QFAVKInstance::validationLayers.size());
    createInfo.ppEnabledLayerNames = QFAVKInstance::validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif


    /////////
    if (vkCreateDevice(QFAVKPhysicalDevice::GetDevice(), &createInfo, nullptr, &Device) != VK_SUCCESS)
        stopExecute("failed to create logical device!");



    vkGetDeviceQueue(Device, indices.graphicsFamily.value(), 0, &GraphicsQueue);
    vkGetDeviceQueue(Device, indices.presentFamily.value(), 0, &PresentQueue);

    IsInit = true;
}
