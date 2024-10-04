#include "pch.h"
#include "Pipline.h"
#include <iostream>
#include <fstream>
#include <EngineStuff/vk/LogicalDevice.h>
#include <Overlord/Time.h>
#include <Tools/File/FileSystem.h>

std::vector<QFAVKPipeline::SShaderData> QFAVKPipeline::ShaderData;

QFAVKPipeline::QFAVKPipeline(QFAPipelineCreateInfo& PipInfo)
{  
    const uint32_t* vertShaderCode = nullptr;
    const uint32_t* fragShaderCode = nullptr;
    uint32_t stageCount = 0;
    
    VkShaderModule vertShaderModule{};    
    VkShaderModule fragShaderModule{};
    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0].flags = shaderStages[1].flags = 0;
    
    if (PipInfo.DescriptorSetLayoutCount > MaxDescriptorSetLayoutCount)
        stopExecute("QFAPipelineCreateInfo::DescriptorSetLayoutCount >  MaxDescriptorSetLayoutCount. Need increase QFAVKPipeline::MaxDescriptorSetLayoutCount");
    
    for (size_t i = 0; i < PipInfo.DescriptorSetLayoutCount; i++)
    {        
        if (PipInfo.DescriptorSetLayouts[i].BindingCount > MaxDescriptorSetLayoutBinding)
            stopExecute("QFAPipelineCreateInfo::DescriptorSetLayouts[i].BindingCount > MaxDescriptorSetLayoutBinding. Need increase QFAVKPipeline::MaxDescriptorSetLayoutBinding");
    }   

    if(PipInfo.ColorBlendState.attachmentCount > MaxColorBlendAttachment)
        stopExecute("QFAPipelineCreateInfo::ColorBlendState.attachmentCount > MaxColorBlendAttachment. Need increase QFAVKPipeline::MaxColorBlendAttachment")


    if (PipInfo.PipelineShaderStages.VertexShaderName.size() > 0 && PipInfo.PipelineShaderStages.FragmentShaderName.size() > 0)
    {
        stageCount = 2;
        SShaderData *sdVert = nullptr;
        SShaderData *sdFrag = nullptr;
        
        for (size_t i = 0; i < ShaderData.size(); i++)
        {
            if (!sdVert && ShaderData[i].name == PipInfo.PipelineShaderStages.VertexShaderName)
            {
                sdVert = &ShaderData[i];
                if (sdFrag)
                    break;
            }
            else if(!sdFrag && ShaderData[i].name == PipInfo.PipelineShaderStages.FragmentShaderName)
            {
                sdFrag = &ShaderData[i];
                if (sdVert)
                    break;
            }
        }

        if (!sdVert || !sdFrag) 
            stopExecute("QFAVKPipeline::QFAVKPipeline shader not found");

        if (sdVert->loadFromFile)
        {
            QFAFile file;
            file.DisableAutoDeleteFileData();
            QFAFileSystem::LoadFile(sdVert->path, &file);
            sdVert->loadFromFile = false;
            sdVert->module = (uint32_t*)file.GetData();
            sdVert->moduleSize = file.GetFileSize();            
        }

        if (sdFrag->loadFromFile)
        {
            QFAFile file;
            file.DisableAutoDeleteFileData();
            QFAFileSystem::LoadFile(sdFrag->path, &file);
            sdFrag->loadFromFile = false;
            sdFrag->module = (uint32_t*)file.GetData();
            sdFrag->moduleSize = file.GetFileSize();
        }                

        vertShaderModule = createShaderModule(sdVert->module, sdVert->moduleSize);
        fragShaderModule = createShaderModule(sdFrag->module, sdFrag->moduleSize);

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;
    }
    else if (PipInfo.PipelineShaderStages.VertexShaderName.size())
    {
        stageCount = 1;
        SShaderData* sdVert = nullptr;

        for (size_t i = 0; i < ShaderData.size(); i++)
            if (!sdVert && ShaderData[i].name == PipInfo.PipelineShaderStages.VertexShaderName)
                sdVert = &ShaderData[i];

        if (!sdVert )
            stopExecute("QFAVKPipeline::QFAVKPipeline shader not found");

        if (sdVert->loadFromFile)
        {
            QFAFile file;
            file.DisableAutoDeleteFileData();
            QFAFileSystem::LoadFile(sdVert->path, &file);
            sdVert->loadFromFile = false;
            sdVert->module = (uint32_t*)file.GetData();
            sdVert->moduleSize = file.GetFileSize();
        }

        vertShaderModule = createShaderModule(sdVert->module, sdVert->moduleSize);

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;
    }
    else
        stopExecute("QFAVKPipeline::QFAVKPipeline");
    

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    vertexInputInfo.vertexBindingDescriptionCount = PipInfo.VertexInputInfo.VertexBindingDescriptionCount;
    vertexInputInfo.vertexAttributeDescriptionCount = PipInfo.VertexInputInfo.VertexAttributeDescriptionCount;
    vertexInputInfo.pVertexBindingDescriptions = PipInfo.VertexInputInfo.VertexBindingDescriptions;
    vertexInputInfo.pVertexAttributeDescriptions = PipInfo.VertexInputInfo.VertexAttributeDescriptions;


    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;


    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = PipInfo.Rasterization.DepthClampEnable;
    rasterizer.rasterizerDiscardEnable = PipInfo.Rasterization.RasterizerDiscardEnable;
    rasterizer.polygonMode = PipInfo.Rasterization.PolygonMode;

    rasterizer.cullMode = PipInfo.Rasterization.CullMode;
    rasterizer.frontFace = PipInfo.Rasterization.FrontFace;
    rasterizer.depthBiasEnable = PipInfo.Rasterization.DepthBiasEnable;
    rasterizer.lineWidth = 1.0f;


    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    
    std::array< VkPipelineColorBlendAttachmentState, MaxColorBlendAttachment> colorBlendAttachments;
    
    for (size_t i = 0; i < PipInfo.ColorBlendState.attachmentCount; i++)
    {
        colorBlendAttachments[i].colorWriteMask = PipInfo.ColorBlendState.pAttachments[i].ColorWriteMask;
        colorBlendAttachments[i].blendEnable = PipInfo.ColorBlendState.pAttachments[i].BlendEnable;
        colorBlendAttachments[i].srcColorBlendFactor = PipInfo.ColorBlendState.pAttachments[i].SrcColorBlendFactor;
        colorBlendAttachments[i].dstColorBlendFactor = PipInfo.ColorBlendState.pAttachments[i].DstColorBlendFactor;
        colorBlendAttachments[i].colorBlendOp = PipInfo.ColorBlendState.pAttachments[i].ColorBlendOp;
        colorBlendAttachments[i].srcAlphaBlendFactor = PipInfo.ColorBlendState.pAttachments[i].SrcAlphaBlendFactor;
        colorBlendAttachments[i].dstAlphaBlendFactor = PipInfo.ColorBlendState.pAttachments[i].DstAlphaBlendFactor;
        colorBlendAttachments[i].alphaBlendOp = PipInfo.ColorBlendState.pAttachments[i].AlphaBlendOp;
    }

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;    
    colorBlending.attachmentCount = PipInfo.ColorBlendState.attachmentCount;    
    colorBlending.pAttachments = colorBlendAttachments.data();
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = PipInfo.DynamicStateCount;
    dynamicState.pDynamicStates = PipInfo.DynamicStates;

    
    CreateDescriptorSetLayouts(PipInfo);
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = PipInfo.DescriptorSetLayoutCount;
    pipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = PipInfo.PushConstant.pushConstantRangeCount;
    pipelineLayoutInfo.pPushConstantRanges = PipInfo.PushConstant.PushConstantRanges;


    if (vkCreatePipelineLayout(QFAVKLogicalDevice::GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
        stopExecute("failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = stageCount;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = PipInfo.RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = PipInfo.DepthStencil.DepthTestEnable;
    depthStencil.depthWriteEnable = PipInfo.DepthStencil.DepthWriteEnable;
    depthStencil.depthCompareOp = PipInfo.DepthStencil.DepthCompareOp;
    depthStencil.depthBoundsTestEnable = PipInfo.DepthStencil.DepthBoundsTestEnable;
    depthStencil.minDepthBounds = PipInfo.DepthStencil.MinDepthBounds;
    depthStencil.maxDepthBounds = PipInfo.DepthStencil.MaxDepthBounds;
    depthStencil.stencilTestEnable = PipInfo.DepthStencil.StencilTestEnable;
    depthStencil.front = PipInfo.DepthStencil.Front;
    depthStencil.back = PipInfo.DepthStencil.Back;

    pipelineInfo.pDepthStencilState = &depthStencil;
    /*
    validation layer: Validation Error: [ VUID-VkPipelineShaderStageCreateInfo-flags-parameter ] | MessageID = 0x17e5c82f 
    | vkCreateGraphicsPipelines(): pCreateInfos[0].pStages[0].flags contains flag bits (0x10) which are not recognized members of
    VkPipelineShaderStageCreateFlagBits. The Vulkan spec states: flags must be a valid combination of VkPipelineShaderStageCreateFlagBits
    values (https://vulkan.lunarg.com/doc/view/1.3.290.0/windows/1.3-extensions/vkspec.html#VUID-VkPipelineShaderStageCreateInfo-flags-parameter)
    */
    if (vkCreateGraphicsPipelines(QFAVKLogicalDevice::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        stopExecute("failed to create graphics pipeline!");

    vkDestroyShaderModule(QFAVKLogicalDevice::GetDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(QFAVKLogicalDevice::GetDevice(), vertShaderModule, nullptr);    

    SetPoolsParameter(PipInfo);

}




void QFAVKPipeline::CreateSet(uint32_t groupIndex, QFADescriptorSetInfo* descriptorInfo)
{
    if (Pools[groupIndex].ListPool.size() == 0 || GroupDescriptorPools[groupIndex].CountSetInLastPool == GroupDescriptorPools[groupIndex].MaxSets)
    {        
        CreatePool(groupIndex);
        GroupDescriptorPools[groupIndex].CountSetInLastPool = 0;
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = Pools[groupIndex].ListPool.back();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = (const VkDescriptorSetLayout*)&DescriptorSetLayouts[groupIndex];

    VkDescriptorSet set;
    if (VkResult res = vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &set))
        stopExecute(res); // "failed to allocate descriptor sets! " 

    Pools[groupIndex].ListSet.push_back(set);
    UpdateSet(groupIndex, (uint32_t)Pools[groupIndex].ListSet.size() - 1, descriptorInfo);
    GroupDescriptorPools[groupIndex].CountSetInLastPool++;
}

void QFAVKPipeline::UpdateSet(uint32_t groupIndex, uint32_t setIndex,  QFADescriptorSetInfo* descriptorInfo)
{
    std::array<VkWriteDescriptorSet, MaxDescriptorSetLayoutBinding> descriptorWrites{};
    for (size_t j = 0; j < GroupDescriptorPools[groupIndex].DescriptorPoolSizeCount; j++)
    {
        descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[j].dstSet = Pools[groupIndex].ListSet[setIndex];
        descriptorWrites[j].dstBinding = descriptorInfo[j].dstBinding;
        descriptorWrites[j].dstArrayElement = 0;
        descriptorWrites[j].descriptorType = GroupDescriptorPools[groupIndex].DescriptorPoolSizes[j].type;
        descriptorWrites[j].descriptorCount = GroupDescriptorPools[groupIndex].DescriptorPoolSizes[j].descriptorCount;
        descriptorWrites[j].pBufferInfo = descriptorInfo[j].DescriptorBufferInfos;
        descriptorWrites[j].pImageInfo = descriptorInfo[j].DescriptorImageInfos;
    }

    vkUpdateDescriptorSets(QFAVKLogicalDevice::GetDevice(), GroupDescriptorPools[groupIndex].DescriptorPoolSizeCount, descriptorWrites.data(), 0, nullptr);
}

std::vector<char> QFAVKPipeline::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        stopExecute("failed to open file!");

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule QFAVKPipeline::createShaderModule(const uint32_t* code, size_t size)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = code;

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(QFAVKLogicalDevice::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        stopExecute("failed to create shader module!");

    return shaderModule;
}

void QFAVKPipeline::SetPoolsParameter(QFAPipelineCreateInfo& PipInfo)
{
    for (size_t i = 0; i < PipInfo.DescriptorSetLayoutCount/*groupCount*/; i++)
    {
        GroupDescriptorPools[i].DescriptorPoolSizeCount = PipInfo.DescriptorSetLayouts[i].BindingCount; // groups[i].DescriptorPoolSizeCount;        
        GroupDescriptorPools[i].MaxSets = PipInfo.MaxSets[i];
        GroupDescriptorPools[i].descriptorPoolFlags = PipInfo.DescriptorSetLayouts[i].descriptorPoolFlags;

        for (size_t j = 0; j < GroupDescriptorPools[i].DescriptorPoolSizeCount; j++)
        {
            GroupDescriptorPools[i].DescriptorPoolSizes[j].descriptorCount = PipInfo.DescriptorSetLayouts[i].Bindings[j].descriptorCount;
            GroupDescriptorPools[i].DescriptorPoolSizes[j].type = PipInfo.DescriptorSetLayouts[i].Bindings[j].descriptorType;            
        }
    }
}

void QFAVKPipeline::CreatePool(uint32_t groupIndex)
{
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = GroupDescriptorPools[groupIndex].DescriptorPoolSizeCount;
    poolInfo.pPoolSizes = GroupDescriptorPools[groupIndex].DescriptorPoolSizes;
    poolInfo.maxSets = GroupDescriptorPools[groupIndex].MaxSets;
    poolInfo.flags = GroupDescriptorPools[groupIndex].descriptorPoolFlags;

        VkDescriptorPool pool;
    if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
        stopExecute("failed to create descriptor pool!");

    Pools[groupIndex].ListPool.push_back(pool);
}

void QFAVKPipeline::CreateDescriptorSetLayouts(QFAPipelineCreateInfo& PipInfo)
{
    for (size_t i = 0; i < PipInfo.DescriptorSetLayoutCount; i++)
    {   
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = PipInfo.DescriptorSetLayouts[i].BindingCount;
        layoutInfo.pBindings = PipInfo.DescriptorSetLayouts[i].Bindings;
        layoutInfo.flags = PipInfo.DescriptorSetLayouts[i].descriptorSetLayoutFlags;

        if (PipInfo.DescriptorSetLayouts[i].pBindingFlags)
        {
            VkDescriptorSetLayoutBindingFlagsCreateInfo dslbfci;
            dslbfci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
            dslbfci.bindingCount = PipInfo.DescriptorSetLayouts[i].BindingCount;
            dslbfci.pBindingFlags = PipInfo.DescriptorSetLayouts[i].pBindingFlags;
            dslbfci.pNext = nullptr;
            layoutInfo.pNext = &dslbfci;
        }
        else
            layoutInfo.pNext = nullptr;

        if (vkCreateDescriptorSetLayout(QFAVKLogicalDevice::GetDevice(), &layoutInfo, nullptr, &DescriptorSetLayouts[i]) != VK_SUCCESS)
            stopExecute("failed to create descriptor set layout!");
    }
}

void QFAVKPipeline::SetShaderData(std::vector<SShaderData> shaderData)
{
    ShaderData = shaderData;
}

QFAVKPipeline::~QFAVKPipeline()
{
    for (size_t i = 0; i < Pools.size(); i++)
        for (size_t j = 0; j < Pools[i].ListPool.size(); j++)
            vkDestroyDescriptorPool(QFAVKLogicalDevice::GetDevice(), Pools[i].ListPool[j], nullptr);
}