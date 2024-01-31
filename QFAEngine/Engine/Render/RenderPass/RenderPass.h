#pragma once
#include <vulkan/vulkan.h>
class QFAWindow;

class QFAVKRenderPass
{

	friend QFAWindow;
	VkRenderPass renderPass;	
public:
	QFAVKRenderPass(VkFormat imageFormat, bool present);
	~QFAVKRenderPass();

private:

};
