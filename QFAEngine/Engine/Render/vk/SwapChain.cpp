#include "SwapChain.h"
#include <Render/vk/QueueFamilies.h>
#include <algorithm>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/TextureImage.h>
#include <Render/vk/VKBuffer.h>
#include <Render/Window/Window.h>

QFAVKSwapChain::QFAVKSwapChain(GLFWwindow* window, VkSurfaceKHR surface, VkCommandPool _commandPool)
{ 
    Window = window;
    Surface = surface;
    commandPool = _commandPool;
    
    swapChainSupport = querySwapChainSupport(QFAVKPhysicalDevice::GetDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
        imageCount = swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface; 

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QFAVKStruct::QueueFamilyIndices indices = QFAVKQueueFamilies::findQueueFamilies(QFAVKPhysicalDevice::GetDevice(), surface);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    
    if (vkCreateSwapchainKHR(QFAVKLogicalDevice::GetDevice(), &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
        stopExecute("failed to create swap chain!");
        
    

    vkGetSwapchainImagesKHR(QFAVKLogicalDevice::GetDevice(), SwapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(QFAVKLogicalDevice::GetDevice(), SwapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    CreateImageViews();
    createDepthResources();
}

QFAVKSwapChain::~QFAVKSwapChain()
{
    if(!glfwWindowShouldClose(Window))
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(Window, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(Window, &width, &height);
            glfwWaitEvents();
        }
    }

    vkDeviceWaitIdle(QFAVKLogicalDevice::GetDevice());
    for (auto framebuffer : swapChainFramebuffers)
        vkDestroyFramebuffer(QFAVKLogicalDevice::GetDevice(), framebuffer, nullptr);

    for (auto imageView : swapChainImageViews)
        vkDestroyImageView(QFAVKLogicalDevice::GetDevice(), imageView, nullptr);

    vkDestroySwapchainKHR(QFAVKLogicalDevice::GetDevice(), SwapChain, nullptr);
}

QFAVKStruct::SwapChainSupportDetails QFAVKSwapChain::querySwapChainSupport(VkPhysicalDevice device)
{
    QFAVKStruct::SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, nullptr);

    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR QFAVKSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) 
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;

    return availableFormats[0];
}

VkPresentModeKHR QFAVKSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{ 
    //return VK_PRESENT_MODE_IMMEDIATE_KHR;
    for (const auto& availablePresentMode : availablePresentModes) 
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
            return availablePresentMode;

    
    return VK_PRESENT_MODE_FIFO_KHR; 
}

VkExtent2D QFAVKSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else 
    {
        int width, height;
        glfwGetFramebufferSize(Window, &width, &height);

        VkExtent2D actualExtent = 
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}



void QFAVKSwapChain::CreateImageViews()
{
    swapChainImageViews.resize(swapChainImages.size());

    for (size_t i = 0; i < swapChainImages.size(); i++)
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkImageView QFAVKSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{ 
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;


    viewInfo.subresourceRange.aspectMask = aspectFlags;
    
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(QFAVKLogicalDevice::GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        stopExecute("failed to create texture image view!");

    return imageView;
}

void QFAVKSwapChain::createDepthResources()
{
    // 
    
    createImage(swapChainExtent.width, swapChainExtent.height, QFAWindow::depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
    depthImageView = createImageView(depthImage, QFAWindow::depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    
}



void QFAVKSwapChain::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{// findMemoryType
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(QFAVKLogicalDevice::GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
        stopExecute("QFAVKTextureImage::createImage failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(QFAVKLogicalDevice::GetDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = QFAVKBuffer::findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(QFAVKLogicalDevice::GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        stopExecute("QFAVKTextureImage::createImage failed to allocate image memory!");

    vkBindImageMemory(QFAVKLogicalDevice::GetDevice(), image, imageMemory, 0);



}

void QFAVKSwapChain::createFramebuffers(VkRenderPass renderPass)
{
    swapChainFramebuffers.resize(swapChainImageViews.size());
    for (size_t i = 0; i < swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = 
        {
            swapChainImageViews[i],
            depthImageView
        };
        // next code move in QFAVFFramebuffer
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(QFAVKLogicalDevice::GetDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
            stopExecute("failed to create framebuffer!");
    }
}




