#pragma once
#include <vulkan/vulkan.h>
class QFAWindow;

class QFAVKRenderPassDepth
{

	friend QFAWindow;
	VkRenderPass renderPass;	
public:
	QFAVKRenderPassDepth();
	~QFAVKRenderPassDepth();

private:

};
