#pragma once
#include <Tools/Debug/VulkanSuff.h>

class QFAVKMeshRenderPass
{
public:
	QFAVKMeshRenderPass();
	~QFAVKMeshRenderPass();

	void CreateRenderPass(bool Clear);

	VkRenderPass renderPass = nullptr;
private:

};
