#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

// like shader program in openGL
class QFAText;
class QFAVKTextPipeline
{
	friend QFAText;
	/*
	

	void RecreateDescriptorPool();

	*/

	// index == -1 updata all DescriptorSet
	
	VkVertexInputBindingDescription getBindingDescription();
	std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();

	//int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::array< VkDescriptorSetLayout, 2> descriptorSetLayouts;
	QFAVKTextPipeline(VkRenderPass rPas, const std::string vertexShaderPath, const std::string fragmentShaderPath);
	~QFAVKTextPipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// move in frameBuffer or not
	void createDescriptorSetLayout();

};




