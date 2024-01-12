#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

// like shader program in openGL
class QFAVKMeshShadowPipeline
{
	
	
	int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSetLayout descriptorSetLayout;
	QFAVKMeshShadowPipeline(VkRenderPass rPas, const std::string vertexShaderPath);
	~QFAVKMeshShadowPipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);


	VkVertexInputBindingDescription getBindingDescription();
	

	std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions();

	// move in frameBuffer or not
	void createDescriptorSetLayout();
};

