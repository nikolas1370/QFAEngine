#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

class QFAWindow;
class QFAPresentImagePipeline
{
	friend QFAWindow;
	void createDescriptorPool();
	void createDescriptorSets();

	// index == -1 updata all DescriptorSet
	void updataDescriptorSets();
	VkVertexInputBindingDescription getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions();
	
	int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;
	QFAPresentImagePipeline(VkRenderPass rPas, const std::string vertexShaderPath, const std::string fragmentShaderPath);
	~QFAPresentImagePipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	void createDescriptorSetLayout();
	VkDescriptorPool descriptorPool;
	static VkDescriptorSet descriptorSet;
};




