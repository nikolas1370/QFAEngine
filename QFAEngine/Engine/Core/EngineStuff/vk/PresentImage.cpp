﻿#include "pch.h"
#include "PresentImage.h"
#include <iostream>
#include <Core/EngineStuff/vk/LogicalDevice.h>
#include <Core/EngineStuff/Image.h>
#include <Core/EngineStuff/Buffer/VertexBuffer.h>
#include <Core/EngineStuff/Pipline/Pipline.h>
#include <Core/EngineStuff/vk/ImageView.h>
#include <Core/EngineStuff/vk/TextureSampler.h>
#include <Core/EngineStuff/Pipline/Pipline.h>



std::array< VkDescriptorImageInfo, 1> QFAPresentImage::imageInfos;

VkCommandPool QFAPresentImage::commandPool;

QFAVKTextureSampler* QFAPresentImage::ImageSampler = nullptr;


QFAPresentImage::QFAPresentImage(VkCommandPool _commandPool, VkRenderPass renderPass, QFAImage* image, VkImageAspectFlags aspect)
{
    commandPool = _commandPool;
    // left top
    quad[0]; 
    quad[0].textureX = 0;
    quad[0].textureY = 0;
    quad[0].x = -1; // 0.5
    quad[0].y = -1;

    // left bottom
    quad[1].textureX = 0;
    quad[1].textureY = 1;
    quad[1].x = -1; // 0.5
    quad[1].y = 1;

    

    // right top
    quad[2];
    quad[2].textureX = 1;
    quad[2].textureY = 0;
    quad[2].x = 1;
    quad[2].y = -1;

    // right bottom
    quad[3];
    quad[3].textureX = 1;
    quad[3].textureY = 1;
    quad[3].x = 1;
    quad[3].y = 1;

    quad[4] = quad[2];


    quad[5] = quad[1];


    vertexBufer = new QFAVKVertexBuffer(sizeof(ImageShaderVertex) * 6, &quad, commandPool);


    QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
    PipelineInfo.RenderPass = renderPass;
    PipelineInfo.PipelineShaderStages.VertexShaderName = U"PresentImage.spvv";
    PipelineInfo.PipelineShaderStages.FragmentShaderName = U"PresentImage.spvf";


    PipelineInfo.VertexInputInfo.VertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(float) * 4;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    PipelineInfo.VertexInputInfo.VertexBindingDescriptions = &bindingDescription;


    PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = 1;
    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[0].offset = 0;
    PipelineInfo.VertexInputInfo.VertexAttributeDescriptions = attributeDescriptions.data();


    PipelineInfo.Rasterization.CullMode = VK_CULL_MODE_NONE;


    QFAVKPipeline::QFAPipelineColorBlendAttachment blendAttachment;
    blendAttachment.BlendEnable = VK_FALSE;

    PipelineInfo.ColorBlendState.attachmentCount = 1;
    PipelineInfo.ColorBlendState.pAttachments = &blendAttachment;


    std::array<VkDynamicState, 2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    PipelineInfo.DynamicStates = dynamicStates.data();
    PipelineInfo.DynamicStateCount = (uint32_t)dynamicStates.size();


    std::array< QFAVKPipeline::QFADescriptorSetLayout, 1> DescriptorSetLayouts;
    std::array<VkDescriptorSetLayoutBinding, 1> bindingsFragment;
    bindingsFragment[0].binding = 0;
    bindingsFragment[0].descriptorCount = 1;
    bindingsFragment[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingsFragment[0].pImmutableSamplers = nullptr;
    bindingsFragment[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    DescriptorSetLayouts[0].BindingCount = (uint32_t)bindingsFragment.size();
    DescriptorSetLayouts[0].Bindings = bindingsFragment.data();

    PipelineInfo.DescriptorSetLayoutCount = (uint32_t)DescriptorSetLayouts.size();
    PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();


    std::array< uint32_t, 1> MaxSets;
    MaxSets[0] = 1;
    PipelineInfo.MaxSets = MaxSets.data();



    Pipeline = new QFAVKPipeline(PipelineInfo);

    if(!ImageSampler)
        ImageSampler = new QFAVKTextureSampler();

    view = new QFAVKImageView(image, aspect);

    imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfos[0].imageView = view->ImageView;
    imageInfos[0].sampler = ImageSampler->textureSampler;

    QFAVKPipeline::QFADescriptorSetInfo info;
    info.dstBinding = 0;
    info.DescriptorImageInfos = imageInfos.data();
    Pipeline->CreateSet(0, &info);
}

QFAPresentImage::~QFAPresentImage()
{
    //delete vertexBufer;
    delete Pipeline;
}

void QFAPresentImage::EndLife()
{    
    delete ImageSampler;
}
