#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAEngineWindow;

class QFAEXPORT QFAVKRenderPassDepth
{
	friend QFAEngineWindow;
	VkRenderPass renderPass;	

public:
	QFAVKRenderPassDepth();
	~QFAVKRenderPassDepth();
};
