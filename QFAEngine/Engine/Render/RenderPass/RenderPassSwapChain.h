#pragma once
#include <vulkan/vulkan.h>
class QFAWindow;

class QFAVKRenderPassSwapChain
{

	friend QFAWindow;
	VkRenderPass renderPass;	
public:
	QFAVKRenderPassSwapChain(VkFormat imageFormat, bool present);
	~QFAVKRenderPassSwapChain();

private:

};
