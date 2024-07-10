#pragma once
#include <EngineStuff/VulkanSuff.h>

class QFAEXPORT QFAVKMeshRenderPass
{
public:
	VkRenderPass renderPass = nullptr;

	QFAVKMeshRenderPass();
	~QFAVKMeshRenderPass();

	void CreateRenderPass(bool Clear, VkFormat imageFormat);	
};
