#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAWindow;

class QFAEXPORT QFAVKRenderPassSwapChain
{
	friend QFAWindow;
	VkRenderPass renderPass;	

public:
	QFAVKRenderPassSwapChain(VkFormat imageFormat, bool present);
	~QFAVKRenderPassSwapChain();
};
