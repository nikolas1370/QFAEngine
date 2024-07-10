#pragma once
#include <EngineStuff/VulkanSuff.h>
#include <string>
#include <array>
#include <vector>

class QFAOverlord; // in QFAEditor
class QFAVKPipeline
{    
    friend QFAOverlord;
    static const uint32_t MaxDescriptorSetLayoutBinding = 4; // if need more VkDescriptorSetLayoutBinding in SetLayout
    struct QFAGroupPools
    {
        VkDescriptorPoolSize DescriptorPoolSizes[QFAVKPipeline::MaxDescriptorSetLayoutBinding];
        uint32_t DescriptorPoolSizeCount = 0;
        uint32_t MaxSets = 0;
        uint32_t CountSetInLastPool = 0;
        VkDescriptorPoolCreateFlags descriptorPoolFlags = 0;
    };

    struct SDescriptorPool
    {
        std::vector< VkDescriptorPool> ListPool;
        std::vector< VkDescriptorSet> ListSet;
    };

public:
    /*
      if loadFromFile == true
          SPIR-V module load from disk(ShaderData::path)
      else
          SPIR-V module store in ShaderData::module
  */
    struct SShaderData
    {
        std::u32string path;
        std::u32string name;
        const uint32_t* module = nullptr;
        size_t moduleSize;
        bool loadFromFile;
    };

    /*
        VertexStage/FragmentStage name shader file
    */
    struct QFAPipelineShaderStages
    {
        std::u32string VertexShaderName;
        std::u32string FragmentShaderName;
    };

    struct QFAVertexInputInfo
    {
        uint32_t VertexBindingDescriptionCount = 0;
        VkVertexInputBindingDescription* VertexBindingDescriptions = nullptr;
        uint32_t VertexAttributeDescriptionCount = 0;
        VkVertexInputAttributeDescription* VertexAttributeDescriptions = nullptr;
    };

    struct QFAPipelineRasterization
    {
        VkBool32 DepthClampEnable = VK_FALSE;
        VkBool32 RasterizerDiscardEnable = VK_FALSE;
        VkPolygonMode PolygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace FrontFace = VK_FRONT_FACE_CLOCKWISE;
        VkBool32 DepthBiasEnable = VK_FALSE;
    };

    struct QFAPipelineColorBlendAttachment
    {
        VkColorComponentFlags ColorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        VkBool32 BlendEnable = VK_FALSE;
        VkBlendFactor SrcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        VkBlendFactor DstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp ColorBlendOp = VK_BLEND_OP_ADD;
        VkBlendFactor SrcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        VkBlendFactor DstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp AlphaBlendOp = VK_BLEND_OP_ADD;
    };

    struct QFAPipelineColorBlendState
    {
        uint32_t attachmentCount = 0;
        QFAPipelineColorBlendAttachment* pAttachments = nullptr;
    };



    struct QFAPipelineDepthStencil
    {
        VkBool32 DepthTestEnable = VK_FALSE;
        VkBool32 DepthWriteEnable = VK_FALSE;
        VkCompareOp DepthCompareOp = VK_COMPARE_OP_LESS;
        VkBool32 DepthBoundsTestEnable = VK_FALSE;
        VkBool32 StencilTestEnable = VK_FALSE;
        VkStencilOpState Front{};
        VkStencilOpState Back{};
        float MinDepthBounds = 0.0f;
        float MaxDepthBounds = 1.0f;
    };

    struct QFADescriptorSetLayout
    {
        uint32_t BindingCount = 0;
        VkDescriptorSetLayoutBinding* Bindings = nullptr;
        VkDescriptorBindingFlags* pBindingFlags = nullptr; // optional
        VkDescriptorSetLayoutCreateFlags descriptorSetLayoutFlags = 0; // optional
        /*
            if in pBindingFlags any binding has the VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
                in descriptorSetLayoutFlags should be VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT
        */
        VkDescriptorPoolCreateFlags descriptorPoolFlags = 0;
        /*
            if descriptorSetLayoutFlags ==
                VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT
                DescriptorPoolFlags should be VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
        */
    };
    struct QFAPushConstant
    {
        uint32_t pushConstantRangeCount = 0;
        VkPushConstantRange* PushConstantRanges = nullptr;
    };

    struct QFAPipelineCreateInfo
    {
        VkRenderPass RenderPass;
        QFAPipelineShaderStages PipelineShaderStages;
        QFAVertexInputInfo VertexInputInfo;
        QFAPipelineRasterization Rasterization;
        QFAPipelineColorBlendState ColorBlendState;
        uint32_t DynamicStateCount = 0;
        VkDynamicState* DynamicStates = nullptr;
        QFAPipelineDepthStencil DepthStencil;
        uint32_t DescriptorSetLayoutCount = 0;
        QFADescriptorSetLayout* DescriptorSetLayouts = nullptr;
        uint32_t* MaxSets = nullptr; // MaxSets[0] == GroupDescriptorPools[0].MaxSets
        QFAPushConstant PushConstant;
    };


    struct QFADescriptorSetInfo
    {
        uint32_t dstBinding = 0;
        VkDescriptorBufferInfo* DescriptorBufferInfos = nullptr;
        VkDescriptorImageInfo* DescriptorImageInfos = nullptr;
    };

public:
    static const uint32_t MaxDescriptorSetLayoutCount = 2;// if need more SetLayout    
    static const uint32_t MaxColorBlendAttachment = 2;

private:
    static std::vector<SShaderData> ShaderData;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    
    std::array< VkDescriptorSetLayout, MaxDescriptorSetLayoutCount> DescriptorSetLayouts;
    std::array<QFAGroupPools, MaxDescriptorSetLayoutCount> GroupDescriptorPools;
    std::array<SDescriptorPool, MaxDescriptorSetLayoutCount> Pools;
    
    static std::vector<char> readFile(const std::string& filename);
    /*
        call in QFAOverlord::Init
    */
    static void SetShaderData(std::vector<SShaderData> shaderData);


    VkShaderModule createShaderModule(const uint32_t* code, size_t size);
    void CreateDescriptorSetLayouts(QFAPipelineCreateInfo& PipInfo);
    void CreatePool(uint32_t groupIndex);    
    void SetPoolsParameter(QFAPipelineCreateInfo& PipInfo);

public:    
    QFAVKPipeline(QFAPipelineCreateInfo& PipInfo);
	~QFAVKPipeline();
    /*
    
        allocate new set if pool end create new pool

    */
    void CreateSet(uint32_t groupIndex, QFADescriptorSetInfo* DescriptorInfo);
    void UpdateSet(uint32_t groupIndex, uint32_t setIndex, QFADescriptorSetInfo* DescriptorInfo);
    
    inline VkDescriptorSet GetSet(uint32_t groupIndex, uint32_t setIndex)
    {        
        return Pools[groupIndex].ListSet[setIndex];
    }

    inline VkPipelineLayout GetPipelineLayout()
    {
        return pipelineLayout;
    }

    inline VkPipeline GetPipeline()
    {
        return graphicsPipeline;
    }
};