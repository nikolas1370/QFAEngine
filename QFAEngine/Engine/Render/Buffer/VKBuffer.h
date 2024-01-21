#pragma once
#include <Tools/VulkanSuff.h>

class QFAWindow;
class QFAVKVertexBuffer;
class QFAVKIndexBuffer;
class QFAImage;
class QFAOverlord;

class QFAVKBuffer
{
	friend QFAOverlord;
	friend QFAImage;
	friend QFAWindow;
	friend QFAVKIndexBuffer;
	friend QFAVKVertexBuffer;

	void CreateBufferInside(VkDeviceSize size, VkBufferUsageFlags usage);

	bool InHost;

	static VmaAllocator allocator;	

	/*
* VmaAllocation stor inside info about memory
* if need see data call vmaGetAllocationInfo
*
store
	VkDeviceMemory deviceMemory
	pMappedData
*/
	VmaAllocation allocation;// 
public:
	static void Init(VkInstance instance);
	/* 
		Buffers represent linear arrays of data which are used for various purposes by binding 
		them to a graphics or compute pipeline via descriptor sets or certain commands, or
		by directly specifying them as parameters to certain commands.
	*/
	VkBuffer Buffer; 

	/* by default, it is created for copying to the second buffers */
	QFAVKBuffer(  VkDeviceSize size, const void* data, bool inHost, VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	~QFAVKBuffer();

	

	void* MapData;

	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	


	
private:	
	void copyBufferToImage(VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	static VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
	static void endSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer commandBuffer);
	static void EndLife();
public:
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	void copyInImage(QFAImage* image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageLayout endLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	void copyBuffer(VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);
	void UpdateData(unsigned int size, void* data);
};
