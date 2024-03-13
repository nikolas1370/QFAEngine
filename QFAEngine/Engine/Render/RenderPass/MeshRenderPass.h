#pragma once
#include <Tools/VulkanSuff.h>

class QFAVKMeshRenderPass
{
public:
	QFAVKMeshRenderPass();
	~QFAVKMeshRenderPass();

	void CreateRenderPass(bool Clear, VkFormat imageFormat);

	VkRenderPass renderPass = nullptr;
private:

};
