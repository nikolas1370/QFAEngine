#pragma once
#include <Tools/VulkanSuff.h>
#include <vector>

class QFAWindow;
class QFAVKVertexBuffer;
class QFAVKIndexBuffer;
class QFAImage;
class QFAOverlord;
class QMeshBaseComponent;
class QFAEXPORT QFAVKBuffer
{
	friend QFAOverlord;
	friend QFAImage;
	friend QFAWindow;
	friend QFAVKIndexBuffer;
	friend QFAVKVertexBuffer;
	friend QMeshBaseComponent;

	enum ETaskType
	{
		TTImage,
		TTBuffer,
		TTtransition
	};

	struct BufferTaskOtherThread
	{
		QFAVKBuffer* buffer;
		ETaskType type;
		VkCommandPool commandPool;
		/* BufferInImage */
		QFAImage* image;
		uint32_t width;
		uint32_t height;
		int32_t imageOffsetX;
		int32_t imageOffsetY;
		VkImageAspectFlags aspect;
		VkImageLayout endLayout;
		/* BufferInBuffer */
		VkBuffer dstBuffer;
		VkDeviceSize size;
		VkDeviceSize srcOffset;
		VkDeviceSize dstOffset;
		/* transitionImageLayout */
		VkImage vKImage;
		VkFormat format;
		VkImageLayout oldLayout;
		VkImageLayout newLayout;
	};

	struct SNotNeedBuffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
		bool isHost;
	};

	bool InHost;

	static VmaAllocator allocator;	
	static std::vector<BufferTaskOtherThread> Tasks;
	static std::vector<SNotNeedBuffer> NotNeedBuffers;
	/*
* VmaAllocation stor inside info about memory
* if need see data call vmaGetAllocationInfo
*
store
	VkDeviceMemory deviceMemory
	pMappedData
*/
	VmaAllocation allocation;// 
	void* MapData;
public:
	VkBuffer Buffer;

private:
	static void ProcessTaskFromOtherThread();
	static void DeleteNotNeedBuffer();
	static VkCommandBuffer beginSingleTimeCommands(VkCommandPool commandPool);
	static void endSingleTimeCommands(VkCommandPool commandPool, VkCommandBuffer commandBuffer);
	static void EndLife();

public:
	static void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandPool commandPool, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	static void Init(VkInstance instance);
	static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	void CreateBufferInside(VkDeviceSize size, VkBufferUsageFlags usage);
	void copyBufferToImage(VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
	void copyImageToBuffer(VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

public:
	/* by default, it is created for copying to the second buffers */
	QFAVKBuffer(  VkDeviceSize size, const void* data, bool inHost, VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	~QFAVKBuffer();

	inline void* GetData()
	{
		return MapData;
	}
	void copyInImage(QFAImage* image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageLayout endLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	void copyInBuffer(QFAImage* image, uint32_t width, uint32_t height, VkCommandPool commandPool, int32_t imageOffsetX = 0, int32_t imageOffsetY = 0, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT, VkImageLayout endLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	void copyBuffer(VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);
	void UpdateData(unsigned int size, void* data);
};
