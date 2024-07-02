#pragma once
#include <Tools/VulkanSuff.h>
#include <Render/vk/ImageView.h>
class QFAVKBuffer;
class QFAVKImageView;
class QFAWindow;
/*
	Store image in cpu and gpu side;
*/
class QFAEXPORT QFAImage
{	
	friend QFAVKBuffer;
	friend QFAVKImageView;
	friend QFAWindow;

	
	
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags = 0);
	
	VkFormat ImageFormat;

	unsigned int Width;
	unsigned int Height;

public:
	VkImage TextureImage;
	QFAVKImageView ImageView;
	VmaAllocation ImageAllocation;
	QFAVKBuffer* buffer = nullptr;

	struct SImageCreateInfo
	{
		VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageCreateFlags flags = 0;
		int Width;
		int Height;
		unsigned int channelCount = 4;		
		/*
			if false buffer be create in SetImage(void* pixels)
		*/
		bool createBuffer = true; 
	};

	QFAImage(SImageCreateInfo &ici);
	QFAImage(int Width, int Height, unsigned int channelCount, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageCreateFlags flags = 0);
	// remove
	QFAImage(const std::string src);
	QFAImage(const std::u32string src);
	~QFAImage();
	
	void DeleteImageInCpuSide();

	inline unsigned int GetWidth()
	{
		return Width;
	}

	inline unsigned int GetHeight()
	{
		return Height;
	}

	void SetImage(void* pixels);


private:
	
};

