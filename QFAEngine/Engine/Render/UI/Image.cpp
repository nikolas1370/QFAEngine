#include "Image.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/TextureImage.h>
#include <Render/vk/VertexBuffer.h>
#include <Render/vk/ImagePipeline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>

QFAVKTextureImage* QFAImage::image = nullptr;
VkDescriptorImageInfo QFAImage::imageInfo;



QFAVKImagePipeline* QFAImage::Pipeline;



VkCommandPool QFAImage::commandPool;


QFAVKTextureSampler* QFAImage::ImageSampler;


VkRenderPass QFAImage::RenderPass;

VkBuffer QFAImage::uniformBufferProj;
VkDeviceMemory QFAImage::uniformBufferProjMemory;
void* QFAImage::uniformBufferProjMapped;




QFAImage::QFAImage(VkCommandPool _commandPool)
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

void QFAImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_, QFAVKTextureImage* imago, VkImageAspectFlags aspect)
{
    commandPool = commandPool_;
    RenderPass = renderPass;
 
    ImageSampler = new QFAVKTextureSampler();
    image = new QFAVKTextureImage(commandPool_, "6213157_0.jpg");
    image = imago;


    VkDeviceSize bufferSize = sizeof(glm::mat4);
    QFAVKBuffer::createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferProj, uniformBufferProjMemory);
    vkMapMemory(QFAVKLogicalDevice::GetDevice(), uniformBufferProjMemory, 0, bufferSize, 0, &uniformBufferProjMapped);



    view = new QFAVKImageView(image, aspect);
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = view->ImageView;
    imageInfo.sampler = ImageSampler->textureSampler;

    Pipeline = new QFAVKImagePipeline(RenderPass, "Engine/Shaders/ImageVertex.spv", "Engine/Shaders/ImageFragment.spv");
}

QFAImage::~QFAImage()
{
    delete vertexBufer;
}

void QFAImage::SetPosition(unsigned int x, unsigned int y)
{
    
    Position_x = x;
    Position_y = y;
}


void QFAImage::Destroy()
{
    delete this;
}


void QFAImage::EndLife()
{
    vkUnmapMemory(QFAVKLogicalDevice::GetDevice(), uniformBufferProjMemory);
    vkDestroyBuffer(QFAVKLogicalDevice::GetDevice(), uniformBufferProj, nullptr); // some plase
    vkFreeMemory(QFAVKLogicalDevice::GetDevice(), uniformBufferProjMemory, nullptr);
    
    delete Pipeline;
    delete ImageSampler;  
}

