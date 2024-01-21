#include "PresentImage.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/Image.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Pipline/PresentImagePipeline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>

QFAImage* QFAPresentImage::image = nullptr;
VkDescriptorImageInfo QFAPresentImage::imageInfo;
QFAPresentImagePipeline* QFAPresentImage::Pipeline;
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
 
    ImageSampler = new QFAVKTextureSampler();
    image = imago;

    view = new QFAVKImageView(image, aspect);
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = view->ImageView;
    imageInfo.sampler = ImageSampler->textureSampler;
    // 
    Pipeline = new QFAPresentImagePipeline(RenderPass, "Engine/Shaders/PresentImageVertex.spv", "Engine/Shaders/PresentImageFragment.spv");
    

}

QFAPresentImage::~QFAPresentImage()
{
    delete vertexBufer;
    delete ImageSampler;
}



void QFAPresentImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}
