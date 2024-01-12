#pragma once
#include <Render/vk/TextureImage.h>

class QFAWindow;
class QFAVKShadowFrameBuffer
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

	QFAVKTextureImage* depthImageQFA;
	VkImageView depthImageView;

	static const int shadowResolution = 2000;
	VkFramebuffer Framebuffer;
};
