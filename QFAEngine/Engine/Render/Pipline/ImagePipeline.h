#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

class QFAWindow;
class QFAVKImagePipeline
{
	friend QFAWindow;
	void createDescriptorPool();
	void createDescriptorSets();

	// index == -1 updata all DescriptorSet
	void updataDescriptorSets();
	VkVertexInputBindingDescription getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions();
	void RecreateDescriptorPool();
	int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;
	QFAVKImagePipeline(VkRenderPass rPas, const std::string vertexShaderPath, const std::string fragmentShaderPath);
	~QFAVKImagePipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// move in frameBuffer or not
	void createDescriptorSetLayout();
	VkDescriptorPool descriptorPool;
	static VkDescriptorSet descriptorSet;
};




