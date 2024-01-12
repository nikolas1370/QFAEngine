#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

// like shader program in openGL
class QFAText;
class QFAVKTextPipeline
{
	friend QFAText;
	void createDescriptorPool();
	void createDescriptorSets();

	// index == -1 updata all DescriptorSet
	void updataDescriptorSets();
	VkVertexInputBindingDescription getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	void RecreateDescriptorPool();
	int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;
	QFAVKTextPipeline(VkRenderPass rPas, const std::string vertexShaderPath, const std::string fragmentShaderPath);
	~QFAVKTextPipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// move in frameBuffer or not
	void createDescriptorSetLayout();
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
};




