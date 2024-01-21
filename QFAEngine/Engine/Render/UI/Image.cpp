﻿#include "Image.h"
#include <iostream>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/TextureImage.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Pipline/ImagePipeline.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>

QFAVKTextureImage* QFAImage::image = nullptr;
VkDescriptorImageInfo QFAImage::imageInfo;
QFAVKImagePipeline* QFAImage::Pipeline;
VkCommandPool QFAImage::commandPool;
QFAVKTextureSampler* QFAImage::ImageSampler;
VkRenderPass QFAImage::RenderPass;

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

void QFAImage::Init(VkRenderPass renderPass, VkCommandPool commandPool_)
{
    commandPool = commandPool_;
    RenderPass = renderPass;
 /*
    ImageSampler = new QFAVKTextureSampler();
    image = imago;

    view = new QFAVKImageView(image, aspect);
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.imageView = view->ImageView;
    imageInfo.sampler = ImageSampler->textureSampler;
    // PresentImageFragment
    Pipeline = new QFAVKImagePipeline(RenderPass, "Engine/Shaders/ImageVertex.spv", "Engine/Shaders/ImageFragment.spv");
    */

    
}

QFAImage::~QFAImage()
{
    delete vertexBufer;
    delete ImageSampler;
}



void QFAImage::EndLife()
{    
    delete Pipeline;
    delete ImageSampler;
}

void QFAImage::RecreateCreatePiline()
{
    /*
    OldPipeline = Pipeline;
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(false);
    */
}

void QFAImage::CreatePiline()
{
    /*
    Pipeline = new QFAVKTextPipeline(RenderPass, QFAText::VertexShaderPath, QFAText::FragmentShaderPath);
    CreateTextProjectionPool();
    CreateTextParameterPool(true);
    */
}

void QFAImage::ParentEnable()
{

}

void QFAImage::ParentDisable()
{

}

void QFAImage::ParentAttach()
{

}

void QFAImage::ParentDisconect()
{

}