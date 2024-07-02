#include "pch.h"
#include "UIImage.h"
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
QFAImage* QFAUIImage::VoidImage = nullptr;

VkDescriptorSet QFAUIImage::CurentDescriptorSetProject;

QFAUIImage::QFAUIImage(QFAImage* image, bool iBackground)
{
    CanRender = true;
    Type = QFAUIType::Image;
    IBackground = iBackground;

    vertexBufer = new QFAVKVertexBuffer(sizeof(SImageShaderVertex) * 6, nullptr, commandPool);
    ChangeQuad();
    SetImage(image);    
    if (iBackground)
        PrepareSet();
}

void QFAUIImage::Render(VkCommandBuffer comandebuffer)
{    
    if (CallPrepareSet)
        PrepareSet();

    if (Index < 0 || Index >= ImageIndexs.size())
        return;

    if (ImageIndexs[Index].image != this )
    {
        return;
    } 

    UpdateUniforms();

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
    if (Index < 0 || Index >= ImageIndexs.size() || ImageIndexs[Index].image != this || !Parent)
        return;

    SImageParam ip;
    ip.opacity = ProcessParentOpacity(Opacity, Parent);    
    
    ip.overflow.enable = 0;
    ip.overflow.leftTopX = 0;
    ip.overflow.leftTopY = 0;
    ip.overflow.rightBottomX = 1000000;
    ip.overflow.rightBottomY = 1000000; // if this image use like background need start get overflow in parent->parent

    QFAUIParent* parent = Parent;
    if (((QFAUIUnit*)Parent)->GetUnitType() != QFAUIType::Scroll || IBackground)
        parent = ((QFAUIUnit*)parent)->GetParent();

    ProcessParentOverflow(ip.overflow, parent);

    ip.BackgroundEnable = !Image;
    ip.BackgroundColor = BackgroundColor;

    memcpy(ImageIndexs[Index].buffer->MapData, &ip, sizeof(SImageParam));

    SImageVertexParam IVP = { UnitScroll + Position_y, Position_x};
    memcpy(ImageIndexs[Index].bufferVertex->MapData, &IVP, sizeof(SImageVertexParam));
}



void QFAUIImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_)
{
    commandPool = commandPool_;
    RenderPass = renderPass;    
    ImageSampler = new QFAVKTextureSampler();    
    QFAImage::SImageCreateInfo ici;
    ici.Width = 1;
    ici.Height = 1;
    ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VoidImage = new QFAImage(ici);
    CreatePipeline();  
}

QFAUIImage::~QFAUIImage()
{
    delete vertexBufer;
}


void QFAUIImage::SetCanStretch(bool can)
{
    CanStretch = can;
    ChangeQuad();
}

void QFAUIImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}

void QFAUIImage::CreateProjectionSet(VkBuffer buffer)
{
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(glm::mat4);

    std::array< QFAVKPipeline::QFADescriptorSetInfo, 1> descriptorSetInfo;
    descriptorSetInfo[0].dstBinding = 0;
    descriptorSetInfo[0].DescriptorBufferInfos = &bufferInfo;

    Pipeline->CreateSet(0, descriptorSetInfo.data());    
}





void QFAUIImage::ChangeQuad()
{//     

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    float newGpuW = 0.0f;
    float newGpuH = 0.0f;
    if (Image && !CanStretch && !IBackground)
    {
        float ratio = (float)Image->GetHeight() / (float)Image->GetWidth();        
        if (Width > Height)
        {
            if (ratio > 1) // Image->Height > Image->Width
            {                
                newGpuH = (float)Height;
                newGpuW = (float)Height * ((float)Image->GetWidth() / (float)Image->GetHeight());
                offsetX = ((float)Width - newGpuW) / 2.0f;
            }
            else
            {
                float SelfRatio = (float)Height / (float)Width;
                if (SelfRatio > ratio)
                {
                    newGpuH = (float)Width * ratio;
                    newGpuW = (float)Width;
                    offsetY = ((float)Height - newGpuH) / 2.0f;
                }
                else
                {
                    newGpuH = (float)Height;
                    newGpuW = (float)Height * ((float)Image->GetWidth() / (float)Image->GetHeight());
                    offsetX = ((float)Width - newGpuW) / 2.0f;
                }
            }           
        }
        else
        {
            if (ratio > 1)// Image->Height > Image->Width
            {
                float SelfRatio = (float)Height / (float)Width;
                if (SelfRatio > ratio)
                {
                    newGpuW = Width;
                    newGpuH = Width * ratio;
                    offsetY = ((float)Height - newGpuH) / 2.0f;
                } 
                else
                {
                    newGpuW = (float)Height * ((float)Image->GetWidth() / (float)Image->GetHeight());
                    newGpuH = (float)Height;
                    offsetX = ((float)Width - newGpuW) / 2.0f;
                }
            }
            else
            {
                newGpuH = (float)Width * ratio;
                newGpuW = (float)Width;
                offsetY = ((float)Height - newGpuH) / 2.0f;
            }
            
        }        
    }
    else
    {
        newGpuH = Height;
        newGpuW = Width;
    }

    // left top
    quad[0].textureX = 0.0f;
    quad[0].textureY = 0.0f;
    quad[0].x = offsetX;
    quad[0].y = offsetY;
    quad[0].z = (float)ZIndex;

    // left bottom
    quad[1].textureX = 0.0f;
    quad[1].textureY = 1.0f;
    quad[1].x = offsetX;
    quad[1].y = newGpuH + offsetY;
    quad[1].z = (float)ZIndex;


    // right top
    quad[2].textureX = 1.0f;
    quad[2].textureY = 0.0f;
    quad[2].x = (float)(newGpuW + offsetX);
    quad[2].y = offsetY;
    quad[2].z = (float)ZIndex;

    // right bottom
    quad[3].textureX = 1.0f;
    quad[3].textureY = 1.0f;
    quad[3].x = (float)(newGpuW + offsetX);
    quad[3].y = (float)(newGpuH + offsetY);
    quad[3].z = (float)ZIndex;

    quad[4] = quad[2];

    quad[5] = quad[1];
    vertexBufer->UpdateData(sizeof(quad), &quad);
    UpdateUniforms();
}
QFAVKPipeline* QFAUIImage::GetPipeline()
{
    return Pipeline;
}


void QFAUIImage::SetSizeParent(unsigned int w, unsigned int h)
{
    bool changeQuad = false;
    if (w != Width || h != Height)
        changeQuad = true;

    Width = w;
    Height = h;
    InnerHeight = Height;
    InnerWidth = Width;

    if(changeQuad)
        ChangeQuad();
}

void QFAUIImage::SetPositionParent(int x, int y)
{
    Position_x = x;
    Position_y = y;
}


void QFAUIImage::SetImage(QFAImage* image)
{
    if (!image)
    {
        Image = nullptr;
        return DisableImage();
    }

    Image = image;
    CallPrepareSet = true;
    ChangeQuad();
}

void QFAUIImage::SetBackgroundColor(QFAColor color)
{
    glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
    BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, (float)color.A / 255.0f);
}

void QFAUIImage::SetBackgroundColor(QFAColorB color)
{
    glm::vec3 linear = Math::srgb_to_linear(glm::vec3((float)color.R / 255.0f, (float)color.G / 255.0f, (float)color.B / 255.0f));
    BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, (float)color.A / 255.0f);    
}

void QFAUIImage::SetBackgroundColor(QFAColorF color)
{
    glm::vec3 linear = Math::srgb_to_linear(glm::vec3(color.R, color.G, color.B));
    BackgroundColor = QFAColorF(linear.r, linear.g, linear.b, color.A);
}

void QFAUIImage::ParentEnable()
{
    CallPrepareSet = true;
    ChangeQuad();
}


void QFAUIImage::ParentDisable()
{
    DisableImage();
}

void QFAUIImage::ParentAttach()
{
    CallPrepareSet = true;
    ChangeQuad();
}

void QFAUIImage::ParentDisconect()
{
    DisableImage();
}

void QFAUIImage::PrepareSet()
{    
    CallPrepareSet = false;
    std::array< QFAVKPipeline::QFADescriptorSetInfo, 3> setInfo;
    VkDescriptorImageInfo imageInfo;

    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.sampler = ImageSampler->textureSampler;
    if (Image)
        imageInfo.imageView = Image->ImageView.ImageView;
    else
        imageInfo.imageView = VoidImage->ImageView.ImageView;
    
    setInfo[0].dstBinding = 0;
    setInfo[0].DescriptorImageInfos = &imageInfo;

    if (Index < ImageIndexs.size() && ImageIndexs[Index].image == this)
    {
        VkDescriptorBufferInfo BufferInfo;
        BufferInfo.buffer = ImageIndexs[Index].buffer->Buffer;
        BufferInfo.offset = 0;
        BufferInfo.range = sizeof(SImageParam);

        VkDescriptorBufferInfo BufferInfoVertex;
        BufferInfoVertex.buffer = ImageIndexs[Index].bufferVertex->Buffer;
        BufferInfoVertex.offset = 0;
        BufferInfoVertex.range = sizeof(SImageVertexParam);

        setInfo[1].dstBinding = 1;
        setInfo[1].DescriptorBufferInfos = &BufferInfo;
        setInfo[2].dstBinding = 2;
        setInfo[2].DescriptorBufferInfos = &BufferInfoVertex;
        Pipeline->UpdateSet(1, Index, setInfo.data());
        UpdateUniforms();

        return;
    }

    for (unsigned int i = 0; i < ImageIndexs.size(); i++)
    {
        if (!ImageIndexs[i].image)
        {
            Index = i;
            ImageIndexs[i].image = this;

            VkDescriptorBufferInfo BufferInfo;
            BufferInfo.buffer = ImageIndexs[i].buffer->Buffer;
            BufferInfo.offset = 0;
            BufferInfo.range = sizeof(SImageParam);

            VkDescriptorBufferInfo BufferInfoVertex;
            BufferInfoVertex.buffer = ImageIndexs[Index].bufferVertex->Buffer;
            BufferInfoVertex.offset = 0;
            BufferInfoVertex.range = sizeof(SImageVertexParam);

            setInfo[1].dstBinding = 1;
            setInfo[1].DescriptorBufferInfos = &BufferInfo;
            setInfo[2].dstBinding = 2;
            setInfo[2].DescriptorBufferInfos = &BufferInfoVertex;
            Pipeline->UpdateSet(1, Index, setInfo.data());
            UpdateUniforms();

            return;
        }
    }


    Index = (unsigned int)ImageIndexs.size();
    SImageIndex ii;
    ii.buffer = new QFAVKBuffer(sizeof(SImageParam), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    ii.bufferVertex = new QFAVKBuffer(sizeof(SImageVertexParam), nullptr, true, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    ii.image = this;
    ii.setIndex = Index;
    ImageIndexs.push_back(ii);


    VkDescriptorBufferInfo BufferInfo;
    BufferInfo.buffer = ImageIndexs[Index].buffer->Buffer;
    BufferInfo.offset = 0;
    BufferInfo.range = sizeof(SImageParam);

    VkDescriptorBufferInfo BufferInfoVertex;
    BufferInfoVertex.buffer = ImageIndexs[Index].bufferVertex->Buffer;
    BufferInfoVertex.offset = 0;
    BufferInfoVertex.range = sizeof(SImageVertexParam);

    setInfo[1].dstBinding = 1;
    setInfo[1].DescriptorBufferInfos = &BufferInfo;
    setInfo[2].dstBinding = 2;
    setInfo[2].DescriptorBufferInfos = &BufferInfoVertex;
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
    PipelineInfo.VertexInputInfo.VertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
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

    std::array<QFAVKPipeline::QFAPipelineColorBlendAttachment, 2> blendAttachments;
    blendAttachments[0].BlendEnable = VK_TRUE;
    blendAttachments[1].BlendEnable = VK_FALSE;
    PipelineInfo.ColorBlendState.attachmentCount = blendAttachments.size();
    PipelineInfo.ColorBlendState.pAttachments = blendAttachments.data();


    std::array<VkDynamicState, 2> dynamicStates =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    PipelineInfo.DynamicStates = dynamicStates.data();
    PipelineInfo.DynamicStateCount = (uint32_t)dynamicStates.size();


    std::array< QFAVKPipeline::QFADescriptorSetLayout, 2> DescriptorSetLayouts;
    std::array<VkDescriptorSetLayoutBinding, 1> firsLayout;
    firsLayout[0].binding = 0;
    firsLayout[0].descriptorCount = 1;
    firsLayout[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    firsLayout[0].pImmutableSamplers = nullptr;
    firsLayout[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> secondLayout;

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
    secondLayout[2].binding = 2;
    secondLayout[2].descriptorCount = 1;
    secondLayout[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    secondLayout[2].pImmutableSamplers = nullptr;
    secondLayout[2].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    DescriptorSetLayouts[0].BindingCount = (uint32_t)firsLayout.size();
    DescriptorSetLayouts[1].BindingCount = (uint32_t)secondLayout.size();
    DescriptorSetLayouts[0].Bindings = firsLayout.data();
    DescriptorSetLayouts[1].Bindings = secondLayout.data();

    PipelineInfo.DescriptorSetLayoutCount = (uint32_t)DescriptorSetLayouts.size();
    PipelineInfo.DescriptorSetLayouts = DescriptorSetLayouts.data();

    PipelineInfo.DepthStencil.DepthTestEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthWriteEnable = VK_TRUE;
    PipelineInfo.DepthStencil.DepthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    std::array< uint32_t, 2> MaxSets;
    MaxSets[0] = 10;// view port set
    MaxSets[1] = QFAUIImage::AmountSetsInImageParamPool;
    PipelineInfo.MaxSets = MaxSets.data();

    Pipeline = new QFAVKPipeline(PipelineInfo);
}