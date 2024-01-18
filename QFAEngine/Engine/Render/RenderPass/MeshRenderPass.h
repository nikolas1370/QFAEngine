#pragma once
#include <Tools/VulkanSuff.h>

class QFAVKMeshRenderPass
{
public:
	QFAVKMeshRenderPass();
	~QFAVKMeshRenderPass();

	void CreateRenderPass(bool Clear);

	VkRenderPass renderPass = nullptr;
private:

};
