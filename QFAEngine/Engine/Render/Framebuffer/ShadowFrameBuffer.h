#pragma once
#include <Render/Image.h>

class QFAWindow;
class QFAEXPORT QFAVKShadowFrameBuffer
{
	friend QFAWindow;
public:
	QFAVKShadowFrameBuffer(VkCommandPool commandPool, VkRenderPass renderPass);
	~QFAVKShadowFrameBuffer();
	
	inline static int GetShadowResolution()
	{
		return shadowResolution;
	}
private:

	QFAImage* depthImageQFA;
	VkImageView depthImageView;

	static const int shadowResolution = 2000;
	VkFramebuffer Framebuffer;
};
