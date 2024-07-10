#pragma once
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/vk/ImageView.h>
class QFAVKBuffer;
class QFAVKImageView;
class QFAEngineWindow;
class QFAContentManager;
class QFAVKShadowFrameBuffer;
class QFAVKMeshFrameBuffer;
class QFAUIImage;
class QFAVKSwapChain;
class QFAText;
class QFAFile;

/*
	Store image in cpu and gpu side;
*/
class QFAEXPORT QFAImage
{	
	friend QFAVKBuffer;
	friend QFAVKImageView;
	friend QFAEngineWindow;
	friend QFAContentManager;
	friend QFAVKShadowFrameBuffer;
	friend QFAVKMeshFrameBuffer;
	friend QFAUIImage;
	friend QFAVKSwapChain;
	friend QFAText;

public:
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

private:
	VkFormat ImageFormat;
	unsigned int Width;
	unsigned int Height;
	VkImage TextureImage;
	QFAVKImageView ImageView;
	VmaAllocation ImageAllocation; // set in vmaCreateImage
	QFAVKBuffer* buffer = nullptr;
#if QFA_EDITOR_ONLY
	std::vector<QFAUIImage*> Images;
protected:
	QFAFile* File = nullptr;// need in QFAContentManager::WriteImage(QFAContentManager::AddFile)
#endif


private:
	void CreateImageVK(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags = 0);
#if QFA_EDITOR_ONLY
	/*
		delete this;

		need in QFAContentManager::AddFile
	*/
	void UpdateImage(QFAImage* image);
	void DeleteMeFromList(QFAUIImage* image);
#endif
	QFAImage(SImageCreateInfo& ici);
	QFAImage(int width, int height, unsigned int channelCount, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageCreateFlags flags = 0);
	QFAImage(const std::u32string src);
	~QFAImage();
	
	void DeleteImageInCpuSide();
	
	static void operator delete (void* p)
	{
		delete p;
	}

protected:
#if QFA_EDITOR_ONLY

	static QFAImage* CreateImage(const std::u32string src)
	{
		return new QFAImage(src);
	}

	static QFAImage* CreateImage(SImageCreateInfo& ici)
	{
		return new QFAImage(ici);
	}
	
#endif
	void SetImage(void* pixels);
public:
	inline unsigned int GetWidth()
	{
		return Width;
	}

	inline unsigned int GetHeight()
	{
		return Height;
	}
};

