#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAEngineWindow;

class QFAEXPORT QFAVKRenderPassSwapChain
{
	friend QFAEngineWindow;
	VkRenderPass renderPass;	

public:
	QFAVKRenderPassSwapChain(VkFormat imageFormat, bool present);
	~QFAVKRenderPassSwapChain();
};
