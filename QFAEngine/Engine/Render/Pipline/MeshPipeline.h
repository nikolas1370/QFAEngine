#pragma once
#include <string>
#include <vector>
#include <Render/vk/VKStruct.h>

// like shader program in openGL
class QFAVKMeshPipeline
{
	
	
	int AtlassCount;
public:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::array<VkDescriptorSetLayout, 2> descriptorSetLayout;// add s
	QFAVKMeshPipeline(VkRenderPass rPas, const std::string vertexShaderPath, const std::string fragmentShaderPath);
	~QFAVKMeshPipeline();

private:
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);


	// move in frameBuffer or not
	void createDescriptorSetLayout();
};

