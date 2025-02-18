#include "pch.h"
#include "TextureSampler.h"
#include <Core/EngineStuff/vk/PhysicalDevice.h>
#include <Core/EngineStuff/vk/LogicalDevice.h>

QFAVKTextureSampler::QFAVKTextureSampler()
{    
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(QFAVKPhysicalDevice::GetDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;// VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    
    if (vkCreateSampler(QFAVKLogicalDevice::GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        stopExecute("failed to create texture sampler!");
}

QFAVKTextureSampler::~QFAVKTextureSampler()
{
    vkDestroySampler(QFAVKLogicalDevice::GetDevice(), textureSampler, nullptr);
}
