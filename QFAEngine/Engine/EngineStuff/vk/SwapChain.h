#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "VKStruct.h"
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/Image.h>


class QFAEngineWindow;
struct GLFWwindow;
class QFAVKSwapChain
{
	friend QFAEngineWindow;
		
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
	QFAImage* depthImage;
	VkImageView depthImageView;
private:
	QFAVKStruct::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void CreateImageViews();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void createDepthResources();

public:
	QFAVKSwapChain(GLFWwindow* window, VkSurfaceKHR surface, VkCommandPool _commandPool);
	~QFAVKSwapChain();
	void createFramebuffers(VkRenderPass renderPass);
};
