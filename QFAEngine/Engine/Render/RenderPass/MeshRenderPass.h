#pragma once
#include <Tools/VulkanSuff.h>

class QFAEXPORT QFAVKMeshRenderPass
{
public:
	QFAVKMeshRenderPass();
	~QFAVKMeshRenderPass();

	void CreateRenderPass(bool Clear, VkFormat imageFormat);

	VkRenderPass renderPass = nullptr;
private:

};
