#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAWindow;

class QFAEXPORT QFAVKTextRenderPass
{
	friend QFAWindow;
	VkRenderPass renderPass;	

public:
	QFAVKTextRenderPass(VkFormat imageFormat, bool clear);
	~QFAVKTextRenderPass();
};
