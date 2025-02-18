#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAEngineWindow;

class QFAEXPORT QFAVKTextRenderPass
{
	friend QFAEngineWindow;
	VkRenderPass renderPass;	

public:
	QFAVKTextRenderPass(VkFormat imageFormat, bool clear);
	~QFAVKTextRenderPass();
};
