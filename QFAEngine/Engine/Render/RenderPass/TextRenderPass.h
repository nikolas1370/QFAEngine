#pragma once
#include <vulkan/vulkan.h>
class QFAWindow;

class QFAVKTextRenderPass
{

	friend QFAWindow;
	VkRenderPass renderPass;	
public:
	QFAVKTextRenderPass(VkFormat swapChainImageFormat);
	~QFAVKTextRenderPass();

private:

};