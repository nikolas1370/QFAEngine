#include "Pipline.h"
#include <iostream>
#include <fstream>
#include <Render/vk/LogicalDevice.h>

QFAVKPipeline::QFAVKPipeline(QFAPipelineCreateInfo& PipInfo)
{
    std::vector<char> vertShaderCode;
    std::vector<char> fragShaderCode;
    uint32_t stageCount = 0;
    
    VkShaderModule vertShaderModule{};
    VkShaderModule fragShaderModule{};
    VkPipelineShaderStageCreateInfo shaderStages[2];
    if (PipInfo.PipelineShaderStages.SPIR_V_file)
    {
        if (PipInfo.PipelineShaderStages.VertexStage && PipInfo.PipelineShaderStages.FragmentStage)
        {
            stageCount = 2;
            vertShaderCode = readFile(PipInfo.PipelineShaderStages.VertexStage);
            fragShaderCode = readFile(PipInfo.PipelineShaderStages.FragmentStage);
            vertShaderModule = createShaderModule(vertShaderCode);
            fragShaderModule = createShaderModule(fragShaderCode);

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
        else if(PipInfo.PipelineShaderStages.VertexStage)
        {
            stageCount = 1;
            vertShaderCode = readFile(PipInfo.PipelineShaderStages.VertexStage);
            vertShaderModule = createShaderModule(vertShaderCode);
            shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
            shaderStages[0].module = vertShaderModule;
            shaderStages[0].pName = "main";
            shaderStages[0].pNext = nullptr;
            shaderStages[0].pSpecializationInfo = nullptr;
        }
        else
            stopExecute("QFAVKPipeline::QFAVKPipeline");
    }
    else
        stopExecute("not SPIR_V_file not support");

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


    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = PipInfo.ColorBlendAttachment.ColorWriteMask;
    colorBlendAttachment.blendEnable = PipInfo.ColorBlendAttachment.BlendEnable;
    colorBlendAttachment.srcColorBlendFactor = PipInfo.ColorBlendAttachment.SrcColorBlendFactor;
    colorBlendAttachment.dstColorBlendFactor = PipInfo.ColorBlendAttachment.DstColorBlendFactor;
    colorBlendAttachment.colorBlendOp = PipInfo.ColorBlendAttachment.ColorBlendOp;
    colorBlendAttachment.srcAlphaBlendFactor = PipInfo.ColorBlendAttachment.SrcAlphaBlendFactor;
    colorBlendAttachment.dstAlphaBlendFactor = PipInfo.ColorBlendAttachment.DstAlphaBlendFactor;
    colorBlendAttachment.alphaBlendOp = PipInfo.ColorBlendAttachment.AlphaBlendOp;


    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
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

    if (vkCreateGraphicsPipelines(QFAVKLogicalDevice::GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        stopExecute("failed to create graphics pipeline!");

    vkDestroyShaderModule(QFAVKLogicalDevice::GetDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(QFAVKLogicalDevice::GetDevice(), vertShaderModule, nullptr);    

    SetPoolsParameter(PipInfo);
}


void QFAVKPipeline::SetPoolsParameter(QFAPipelineCreateInfo& PipInfo)
{    
    for (size_t i = 0; i < PipInfo.DescriptorSetLayoutCount/*groupCount*/; i++)
    {        
        GroupDescriptorPools[i].DescriptorPoolSizeCount = PipInfo.DescriptorSetLayouts[i].BindingCount; // groups[i].DescriptorPoolSizeCount;        
        GroupDescriptorPools[i].MaxSets = PipInfo.MaxSets[i];

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

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(QFAVKLogicalDevice::GetDevice(), &poolInfo, nullptr, &pool) != VK_SUCCESS)
        stopExecute("failed to create descriptor pool!");

    Pools[groupIndex].ListPool.push_back(pool);
}

void QFAVKPipeline::CreateSet(uint32_t groupIndex, QFADescriptorSetInfo* descriptorInfo)
{
    if (Pools[groupIndex].ListPool.size() == 0 || GroupDescriptorPools[groupIndex].DescriptorPoolSizeCount == GroupDescriptorPools[groupIndex].MaxSets)
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
    if (vkAllocateDescriptorSets(QFAVKLogicalDevice::GetDevice(), &allocInfo, &set) != VK_SUCCESS)
        stopExecute("failed to allocate descriptor sets!");

    Pools[groupIndex].ListSet.push_back(set);
    UpdateSet(groupIndex, Pools[groupIndex].ListSet.size() - 1, descriptorInfo);
    GroupDescriptorPools[groupIndex].CountSetInLastPool++;
}

void QFAVKPipeline::UpdateSet(uint32_t groupIndex, uint32_t setIndex,  QFADescriptorSetInfo* descriptorInfo)
{
    std::array<VkWriteDescriptorSet, MaxDescriptorPoolSizeCount> descriptorWrites{};
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

VkShaderModule QFAVKPipeline::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(QFAVKLogicalDevice::GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        stopExecute("failed to create shader module!");

    return shaderModule;
}

void QFAVKPipeline::CreateDescriptorSetLayouts(QFAPipelineCreateInfo& PipInfo)
{
    for (size_t i = 0; i < PipInfo.DescriptorSetLayoutCount; i++)
    {        
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = PipInfo.DescriptorSetLayouts[i].BindingCount;
        layoutInfo.pBindings = PipInfo.DescriptorSetLayouts[i].Bindings;

        if (vkCreateDescriptorSetLayout(QFAVKLogicalDevice::GetDevice(), &layoutInfo, nullptr, &DescriptorSetLayouts[i]) != VK_SUCCESS)
            stopExecute("failed to create descriptor set layout!");
    }
}

QFAVKPipeline::~QFAVKPipeline()
{
    for (size_t i = 0; i < Pools.size(); i++)
        for (size_t j = 0; j < Pools[i].ListPool.size(); j++)
            vkDestroyDescriptorPool(QFAVKLogicalDevice::GetDevice(), Pools[i].ListPool[j], nullptr);
}