#include "TextRenderPass.h"
#include <Tools/VulkanSuff.h>
#include <Render/vk/LogicalDevice.h>
#include <Render/Window/Window.h>
QFAVKTextRenderPass::QFAVKTextRenderPass(VkFormat imageFormat, bool clear)
{


    
    std::array< VkAttachmentDescription, 2> colorAttachments{};
    
    colorAttachments[0].format = imageFormat;
    colorAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachments[0].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachments[0].initialLayout = VK_IMAGE_LAYOUT_GENERAL; // VK_IMAGE_LAYOUT_GENERAL; // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    colorAttachments[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL;// VK_IMAGE_LAYOUT_GENERAL;//  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL

    colorAttachments[1].format = VK_FORMAT_R32_UINT;
    colorAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachments[1].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachments[1].initialLayout = VK_IMAGE_LAYOUT_GENERAL; // VK_IMAGE_LAYOUT_GENERAL; // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    colorAttachments[1].finalLayout = VK_IMAGE_LAYOUT_GENERAL;// VK_IMAGE_LAYOUT_GENERAL;//  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL



    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR ;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;//VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    

    //VkAttachmentReference colorAttachmentRef{};
    std::array< VkAttachmentReference, 2> colorAttachmentRefs{};
    colorAttachmentRefs[0].attachment = 0;
    colorAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentRefs[1].attachment = 1;
    colorAttachmentRefs[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 2;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = (uint32_t)colorAttachmentRefs.size();
    subpass.pColorAttachments = colorAttachmentRefs.data();
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    
    std::array<VkAttachmentDescription, 3> attachments = { colorAttachments[0], colorAttachments[1], depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    

    if (vkCreateRenderPass(QFAVKLogicalDevice::GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        stopExecute("failed to create render pass!");
}



QFAVKTextRenderPass::~QFAVKTextRenderPass()
{
    vkDestroyRenderPass(QFAVKLogicalDevice::GetDevice(), renderPass, nullptr);
}
