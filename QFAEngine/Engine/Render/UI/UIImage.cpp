﻿#include "UIImage.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/Image.h>
#include <Render/Buffer/VertexBuffer.h>

#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>
#include <Render/Pipline/Pipline.h>
#include <Render/Window/Window.h>

VkDescriptorImageInfo QFAUIImage::imageInfo;
QFAVKPipeline* QFAUIImage::Pipeline;
VkCommandPool QFAUIImage::commandPool;
QFAVKTextureSampler* QFAUIImage::ImageSampler;
VkRenderPass QFAUIImage::RenderPass;

std::vector<QFAUIImage::SImageIndex> QFAUIImage::ImageIndexs;


VkDescriptorSet QFAUIImage::CurentDescriptorSetProject;







QFAUIImage::QFAUIImage(QFAImage* image)
{
    CanRender = true;
    Type = QFAUIType::Image;

    vertexBufer = new QFAVKVertexBuffer(sizeof(SImageShaderVertex) * 6, nullptr, commandPool);
    ChangeQuad();
    SetImage(image);    
}

void QFAUIImage::Render(VkCommandBuffer comandebuffer)
{
    if (!Image || 
        ImageIndexs[Index].image != this)
    {
        return;
    }

    /*
    
        якшо ця штукк є то є баг де другий вюпорт появляється на першом
        убере рядок і нема бага
    
    */
    

    VkDescriptorSet CurentDescriptorSet = Pipeline->GetSet(1, Index);    

    VkBuffer vertexBuffers[] = { vertexBufer->GpuSideBuffer->Buffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(comandebuffer, 0, 1, vertexBuffers, offsets);

    VkDescriptorSet ar[] = { CurentDescriptorSetProject, CurentDescriptorSet };

    vkCmdBindDescriptorSets(comandebuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        QFAUIImage::Pipeline->GetPipelineLayout(), 0, 2, ar, 0, nullptr);

    vkCmdDraw(comandebuffer, 6, 1, 0, 0);
}

void QFAUIImage::UpdateUniforms()
{
    if (!Image ||
        ImageIndexs[Index].image != this)
    {
        return;
    }

    SImageParam ip;
    ip.opacity = ProcessParentOpacity(Opacity, Parent);

    ip.overflow.enable = 0;
    ip.overflow.leftTopX = 0;
    ip.overflow.leftTopY = 0;
    ip.overflow.rightBottomX = 1000000;
    ip.overflow.rightBottomY = 1000000;

    ProcessParentOverflow(ip.overflow, Parent);
    memcpy(ImageIndexs[Index].buffer->MapData, &ip, sizeof(SImageParam));
}



void QFAUIImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_)
{
    commandPool = commandPool_;
    RenderPass = renderPass;    
    ImageSampler = new QFAVKTextureSampler();    
    CreatePipeline();  
}

QFAUIImage::~QFAUIImage()
{
    delete vertexBufer;
    delete ImageSampler;
}

void QFAUIImage::SetImage(QFAImage* image)
{
    Image = image;
    if (!image)
        return DisableImage();

    PrepareSet();
}



void QFAUIImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}

void QFAUIImage::CreateTextProjectionSets()
{
    for (size_t i = 0; i < QFAWindow::MaxActiveViewPort; i++)
    {
        VkDescriptorBufferInfo bufferInfo;
        bufferInfo.buffer = QFAWindow::MainWindow->ViewportBuffers[i].uiProjectionBuffer->Buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(glm::mat4);

        std::array< QFAVKPipeline::QFADescriptorSetInfo, 1> descriptorSetInfo;
        descriptorSetInfo[0].dstBinding = 0;
        descriptorSetInfo[0].DescriptorBufferInfos = &bufferInfo;

        Pipeline->CreateSet(0, descriptorSetInfo.data());
    }
}





void QFAUIImage::ChangeQuad()
{
    // left top
    quad[0];
    quad[0].textureX = 0;
    quad[0].textureY = 0;
    quad[0].x = Position_x;
    quad[0].y = Position_y;
    quad[0].z = ZIndex;

    // left bottom
    quad[1].textureX = 0;
    quad[1].textureY = 1;
    quad[1].x = Position_x;
    quad[1].y = Position_y + Height;
    quad[1].z = ZIndex;


    // right top
    quad[2];
    quad[2].textureX = 1;
    quad[2].textureY = 0;
    quad[2].x = Position_x + Width;
    quad[2].y = Position_y;
    quad[2].z = ZIndex;

    // right bottom
    quad[3];
    quad[3].textureX = 1;
    quad[3].textureY = 1;
    quad[3].x = Position_x + Width;
    quad[3].y = Position_y + Height;
    quad[3].z = ZIndex;

    quad[4] = quad[2];

    quad[5] = quad[1];
    vertexBufer->UpdateData(sizeof(quad), &quad);
    UpdateUniforms();
}

void QFAUIImage::SetSizeParent(unsigned int w, unsigned int h)
{
    Width = w;
    Height = h;

    ChangeQuad();
}

void QFAUIImage::SetPositionParent(unsigned int x, unsigned int y)
{
    Position_x = x;
    Position_y = y;
    ChangeQuad();
}


void QFAUIImage::ParentEnable()
{
    PrepareSet();
}


void QFAUIImage::ParentDisable()
{
    DisableImage();
}


void QFAUIImage::ParentAttach()
{
    PrepareSet();
}


void QFAUIImage::ParentDisconect()
{
    DisableImage();
}

void QFAUIImage::PrepareSet()
{
    if (!Image)
        return;

    std::array< QFAVKPipeline::QFADescriptorSetInfo, 2> setInfo;
    VkDescriptorImageInfo imageInfo;

    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = Image->ImageView.ImageView;
    imageInfo.sampler = ImageSampler->textureSampler;
    setInfo[0].dstBinding = 0;
    setInfo[0].DescriptorImageInfos = &imageInfo;


    if (Index < ImageIndexs.size() && ImageIndexs[Index].image == this)
    {
        VkDescriptorBufferInfo BufferInfo;
        BufferInfo.buffer = ImageIndexs[Index].buffer->Buffer;
        BufferInfo.offset = 0;
        BufferInfo.range = sizeof(SImageParam);
        setInfo[1].dstBinding = 1;
        setInfo[1].DescriptorBufferInfos = &BufferInfo;
        Pipeline->UpdateSet(1, Index, setInfo.data());
        UpdateUniforms();
        return;
    }

    for (size_t i = 0; i < ImageIndexs.size(); i++)
    {
        if (!ImageIndexs[i].image)
        {
            Index = i;
            ImageIndexs[i].image = this;

            VkDescriptorBufferInfo BufferInfo;
            BufferInfo.buffer = ImageIndexs[i].buffer->Buffer;
            BufferInfo.offset = 0;
            BufferInfo.range = sizeof(SImageParam);
            setInfo[1].dstBinding = 1;
            setInfo[1].DescriptorBufferInfos = &BufferInfo;
            Pipeline->UpdateSet(1, Index, setInfo.data());
            UpdateUniforms();
            return;
        }
    }


    Index = ImageIndexs.size();
    SImageIndex ii;
    ii.buffer = new QFAVKBuffer(sizeof(SImageParam), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    ii.image = this;
    ii.setIndex = Index;
    ImageIndexs.push_back(ii);


    VkDescriptorBufferInfo BufferInfo;
    BufferInfo.buffer = ImageIndexs[Index].buffer->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = sizeof(SImageParam);
    setInfo[1].dstBinding = 1;
    setInfo[1].DescriptorBufferInfos = &BufferInfo;
    UpdateUniforms();
    Pipeline->CreateSet(1, setInfo.data());
}

void QFAUIImage::DisableImage()
{
    if (Index < ImageIndexs.size() && ImageIndexs[Index].image == this)
        ImageIndexs[Index].image = nullptr;
}


void QFAUIImage::CreatePipeline()
{
    QFAVKPipeline::QFAPipelineCreateInfo PipelineInfo;
    PipelineInfo.RenderPass = RenderPass;
    PipelineInfo.PipelineShaderStages.VertexShaderName = U"Image.spvv";
    PipelineInfo.PipelineShaderStages.FragmentShaderName = U"Image.spvf";


    PipelineInfo.VertexInputInfo.VertexBindingDescriptionCount = 1;
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(QFAUIImage::SImageShaderVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    PipelineInfo.VertexInputInfo.VertexBindingDescriptions = &bindingDescription;


    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = attributeDescriptions.size();
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // 
    attributeDescriptions[0].offset = 0;
    
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(SImageShaderVertex, textureX);
    
    PipelineInfo.VertexInputInfo.VertexAttributeDescriptions = attributeDescriptions.data();


    PipelineInfo.Rasterization.CullMode = VK_CULL_MODE_NONE;

    QFAVKPipeline::QFAPipelineColorBlendAttachment blendAttachment;
    blendAttachment.BlendEnable = VK_TRUE;
    PipelineInfo.ColorBlendState.attachmentCount = 1;
    PipelineInfo.ColorBlendState.pAttachments = &blendAttachment;


    std::array<VkDynamicState, 2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    PipelineInfo.DynamicStates = dynamicStates.data();
    PipelineInfo.DynamicStateCount = dynamicStates.size();


    std::array< QFAVKPipeline::QFADescriptorSetLayout, 2> DescriptorSetLayouts;
    std::array<VkDescriptorSetLayoutBinding, 1> firsLayout;
    firsLayout[0].binding = 0;
    firsLayout[0].descriptorCount = 1;
    firsLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    firsLayout[0].pImmutableSamplers = nullptr;
    firsLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> secondLayout;
    secondLayout[0].binding = 0;
    secondLayout[0].descriptorCount = 1;
    secondLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    secondLayout[0].pImmutableSamplers = nullptr;
    secondLayout[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    secondLayout[1].binding = 1;
    secondLayout[1].descriptorCount = 1;
    secondLayout[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    secondLayout[1].pImmutableSamplers = nullptr;
    secondLayout[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    DescriptorSetLayouts[0].BindingCount = firsLayout.size();
    DescriptorSetLayouts[1].BindingCount = secondLayout.size();
    DescriptorSetLayouts[0].Bindings = firsLayout.data();
    DescriptorSetLayouts[1].Bindings = secondLayout.data();

    PipelineInfo.DescriptorSetLayoutCount = DescriptorSetLayouts.size();
    PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();

    PipelineInfo.DepthStencil.DepthTestEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthWriteEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    std::array< uint32_t, 2> MaxSets;
    MaxSets[0] = QFAWindow::MaxActiveViewPort;
    MaxSets[1] = QFAUIImage::AmountSetsInImageParamPool;
    PipelineInfo.MaxSets = MaxSets.data();

    Pipeline = new QFAVKPipeline(PipelineInfo);

    CreateTextProjectionSets();
}