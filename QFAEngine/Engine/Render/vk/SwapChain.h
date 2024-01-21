#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "VKStruct.h"
#include <Tools/VulkanSuff.h>
#include <Render/Image.h>

class QFAWindow;

/* 

	don't know how it work
	
*/
class QFAVKSwapChain
{


	friend QFAWindow;
	
	
	VkSurfaceKHR Surface;
	GLFWwindow *Window;
	
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkSwapchainKHR SwapChain;

	QFAVKStruct::SwapChainSupportDetails swapChainSupport;
	VkCommandPool commandPool;
public:
	

	QFAVKSwapChain(GLFWwindow* window, VkSurfaceKHR surface, VkCommandPool _commandPool);
	~QFAVKSwapChain();
	void createFramebuffers(VkRenderPass renderPass);
private:
	QFAVKStruct::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	

	void CreateImageViews();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	
	void createDepthResources();	
	QFAImage* depthImage;
	VkImageView depthImageView;
};
