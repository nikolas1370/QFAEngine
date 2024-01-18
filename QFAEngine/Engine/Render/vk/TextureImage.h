#pragma once
#include <string>
#include <Tools/VulkanSuff.h>
class QFAVKBuffer;
class QFAVKImageView;

class QFAVKTextureImage
{	
	friend QFAVKBuffer;
	friend QFAVKImageView;

	//Image objects are not directly accessed by pipeline shaders for reading or writing image data.Instead, image views representing contiguous ranges of the image subresources and containing additional metadata are used for that purpose.Views must be created on images of compatible types, and must represent a valid subset of image subresources.
	
	//VkDeviceMemory TextureImageMemory;
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
	
	VkFormat ImageFormat;
public:
	VkImage TextureImage;
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

	QFAVKTextureImage(VkCommandPool commandPool, SImageCreateInfo &ici);
	QFAVKTextureImage(VkCommandPool commandPool, int Width, int Height, unsigned int channelCount, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	QFAVKTextureImage(VkCommandPool commandPool, const std::string src);
	~QFAVKTextureImage();
	


private:
	
};

