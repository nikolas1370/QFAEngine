#include "UIImage.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/Image.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Pipline/ImagePipeline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>

VkDescriptorImageInfo QFAUIImage::imageInfo;
QFAVKImagePipeline* QFAUIImage::Pipeline;
VkCommandPool QFAUIImage::commandPool;
QFAVKTextureSampler* QFAUIImage::ImageSampler;
VkRenderPass QFAUIImage::RenderPass;

QFAUIImage::QFAUIImage(VkCommandPool _commandPool)
{
    Type = QFAUIType::Image;
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

void QFAUIImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_)
{
    commandPool = commandPool_;
    RenderPass = renderPass;    
    ImageSampler = new QFAVKTextureSampler();
 /*


    
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = view->ImageView;
    imageInfo.sampler = ImageSampler->textureSampler;
    // PresentImageFragment
    Pipeline = new QFAVKImagePipeline(RenderPass, "Engine/Shaders/ImageVertex.spv", "Engine/Shaders/ImageFragment.spv");
    */

    
}



QFAUIImage::~QFAUIImage()
{
    delete vertexBufer;
    delete ImageSampler;
}

void QFAUIImage::SetImage(QFAImage* image)
{
    Image = image;
}



void QFAUIImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}

void QFAUIImage::RecreateCreatePiline()
{
    /*
    OldPipeline = Pipeline;
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(false);
    */
}

void QFAUIImage::CreatePiline()
{
    /*
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(true);
    */
}

/*

    нада зробить на перевіркі чи це небуло ложне збрацювання(баг)


*/

void QFAUIImage::ParentEnable()
{

}


void QFAUIImage::ParentDisable()
{

}

/*

    нада зробить на перевіркі чи це небуло ложне збрацювання(баг)


*/
void QFAUIImage::ParentAttach()
{

}


void QFAUIImage::ParentDisconect()
{

}