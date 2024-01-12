#pragma once
#include <Tools/Debug/OpenGlStuff.h>

class QFAVKTextureImage;
class QFAVKBuffer
{
	friend QFAVKTextureImage;
public:
	/* 
		Buffers represent linear arrays of data which are used for various purposes by binding 
		them to a graphics or compute pipeline via descriptor sets or certain commands, or
		by directly specifying them as parameters to certain commands.
	*/
	VkBuffer StagingBuffer; 
	VkDeviceMemory StagingBufferMemory; // handle in which information about the allocated memory is returned.
	/* by default, it is created for copying to the second buffers */
	QFAVKBuffer(  VkDeviceSize size, const void* data, VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	~QFAVKBuffer();

	void* MapData;

	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	void copyInImage(QFAVKTextureImage* image,  uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageLayout endLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	void copyBuffer(VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);
	static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void UpdateData(unsigned int size, void* data);
	
private:
	
	
	// x y image offset
	void copyBufferToImage(VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	static VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
	static void endSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer commandBuffer);

public:
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
};
