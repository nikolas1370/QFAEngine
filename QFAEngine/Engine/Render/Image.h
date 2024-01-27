#pragma once
#include <string>
#include <Tools/VulkanSuff.h>
#include <Render/vk/ImageView.h>
class QFAVKBuffer;
class QFAVKImageView;
class QFAWindow;
/*
	Store image in cpu and gpu side;
*/
class QFAImage
{	
	friend QFAVKBuffer;
	friend QFAVKImageView;
	friend QFAWindow;

	
	
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
	
	VkFormat ImageFormat;
	static VkCommandPool CommandPool;
	static void Init(VkCommandPool commandPool);
public:
	VkImage TextureImage;
	QFAVKImageView ImageView;
	VmaAllocation ImageAllocation;
	QFAVKBuffer* buffer;

	struct SImageCreateInfo
	{
		VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		int Width;
		int Height;
		unsigned int channelCount = 4;
	};

	QFAImage(SImageCreateInfo &ici);
	QFAImage(int Width, int Height, unsigned int channelCount, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	QFAImage(const std::string src);
	~QFAImage();
	
	void DeleteImageInCpuSide();


private:
	
};

