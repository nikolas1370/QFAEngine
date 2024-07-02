#pragma once
#include <vulkan/vulkan.h>
#include <Tools/Stuff.h>
class QFAWindow;

class QFAEXPORT QFAVKRenderPassDepth
{

	friend QFAWindow;
	VkRenderPass renderPass;	
public:
	QFAVKRenderPassDepth();
	~QFAVKRenderPassDepth();

private:

};
