#include "PresentImage.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/Image.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Pipline/Pipline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>
#include <Render/Pipline/Pipline.h>

QFAImage* QFAPresentImage::image = nullptr;

std::array< VkDescriptorImageInfo, 1> QFAPresentImage::imageInfos;
QFAVKPipeline* QFAPresentImage::Pipeline;
VkCommandPool QFAPresentImage::commandPool;
QFAVKTextureSampler* QFAPresentImage::ImageSampler;
VkRenderPass QFAPresentImage::RenderPass;


QFAPresentImage::QFAPresentImage(VkCommandPool _commandPool)
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
}

void QFAPresentImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_, QFAImage* imago, VkImageAspectFlags aspect)
{
    commandPool = commandPool_;
    RenderPass = renderPass;
 
    
    
    QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
    PipelineInfo.RenderPass = RenderPass;
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
    PipelineInfo.DynamicStateCount = dynamicStates.size();


    std::array< QFAVKPipeline::QFADescriptorSetLayout, 1> DescriptorSetLayouts;        
    std::array<VkDescriptorSetLayoutBinding, 1> bindingsFragment;
    bindingsFragment[0].binding = 0;
    bindingsFragment[0].descriptorCount = 1;
    bindingsFragment[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindingsFragment[0].pImmutableSamplers = nullptr;
    bindingsFragment[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    DescriptorSetLayouts[0].BindingCount = bindingsFragment.size();
    DescriptorSetLayouts[0].Bindings = bindingsFragment.data();

    PipelineInfo.DescriptorSetLayoutCount = DescriptorSetLayouts.size();
    PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();


    std::array< uint32_t, 1> MaxSets;
    MaxSets[0] = 1;
    PipelineInfo.MaxSets = MaxSets.data();


    
    Pipeline = new QFAVKPipeline(PipelineInfo);


    ImageSampler = new QFAVKTextureSampler();
    image = imago;

    view = new QFAVKImageView(image, aspect);// 


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
    delete vertexBufer;
    delete ImageSampler;
    delete Pipeline;
}



void QFAPresentImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}
