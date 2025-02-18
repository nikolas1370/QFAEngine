#pragma once
#include <Core/EngineStuff/Image.h>

class QFAEngineWindow;
class QFAEXPORT QFAVKShadowFrameBuffer
{
	friend QFAEngineWindow;

	QFAImage* depthImageQFA;
	VkImageView depthImageView;
	static const int shadowResolution = 2000;
	VkFramebuffer Framebuffer;

public:
	QFAVKShadowFrameBuffer(VkCommandPool commandPool, VkRenderPass renderPass);
	~QFAVKShadowFrameBuffer();
	
	inline static int GetShadowResolution()
	{
		return shadowResolution;
	}
};
