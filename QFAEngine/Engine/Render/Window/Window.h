#pragma once
#include <Tools/Array.h>
#include <Tools/VulkanSuff.h>
#include <Math/Math.h>
#include <Render/vk/VKInstance.h>
#include <Render/vk/PhysicalDevice.h>
#include <Render/vk/LogicalDevice.h>
#include <Render/vk/SwapChain.h>
#include <Render/RenderPass/RenderPass.h>
#include <string>
#include <Math/Vector.h>
#include <Render/vk/QueueFamilies.h>

#include <Render/Image.h>
#include <Render/vk/ImageView.h>
#include <Render/vk/TextureSampler.h>
#include <Render/Buffer/VertexBuffer.h>
#include <Render/Buffer/IndexBuffer.h>

#include <Render/RenderPass/RenderPassDepth.h>
#include <Render/Framebuffer/ShadowFrameBuffer.h>
#include <Render/RenderPass/TextRenderPass.h>
#include <Render/vk/PresentImage.h>
#include <Render/UI/UIUnit.h>

#include <Render/Window/UIEvent.h>
struct UniformBufferObject
{// more detail https://fvcaputo.github.io/2019/02/06/memory-alignment.html
	/*
		An array or structure type has an extended
		alignment equal to the largest extended alignment of any of
		its members, rounded up to a multiple of 16.
	*/
	alignas(16) glm::mat4 projection;
};

struct GLFWwindow;
class QFAViewport;
class QFAOverlord;
class QFAFrameBufferMain;
class QFAFrameBuffer;
class QMeshBaseComponent;
class QCameraComponent;
class QFAImage;
class QFAVKMeshShadowPipeline;
class QFAVKMeshFrameBuffer;
class QActor;
class QSceneComponent;
class QWorld;
class QFAText;
class QFAUIUnit;
class QFAUIImage;
class QFAVKRenderPassSwapChain;
class QFAWindow
{

	friend QFAText;; 
	friend QFAUIImage;
	friend QFAOverlord;
	friend QMeshBaseComponent;	


	static QFAWindow* MainWindow;
	GLFWwindow* glfWindow;
	
	static bool Init;
	int Width;
	int Height;
	int NewHeight;
	int NewWidth;
	bool WindowSizeChanched = false;
	QFAVKInstance*Instance;
	VkSurfaceKHR surface;	
	QFAVKSwapChain *SwapChain;
	QFAVKRenderPassSwapChain* RenderPassSwapChain;
	QFAVKRenderPass* RenderPass;
	QFAVKRenderPassDepth* RenderPassOffScreen;

	QFAVKTextRenderPass* TextRenderPass;

	int countFarame;
	double acumulateDeltatime;
	


	QFAArray<QFAViewport*> Viewports;


	static void ProcessUIEvent();
	void StartFrame();
	

	bool framebufferResized = false;
	static void RenderWindow();	


	/*----*/
	QFAImage* imagu;


	//VkImage swapChainImages;
	VkImageView swapChainImageViews; /// color image



	// VkImage ;


public:
	static const int MaxActiveViewPort = 10;
	unsigned int ViewportProcess = 0;
	
private:
	
	QFAVKShadowFrameBuffer* ShadowFrameBuffer;


	void ShadowRender(QFAViewport* viewport);

	/*----*/

	VkCommandPool commandPool;


	VkSemaphore GetImageSemaphore;	
	std::array<VkSemaphore, QFAWindow::MaxActiveViewPort> ActorFinishedSemi;
	std::array<VkPipelineStageFlags, QFAWindow::MaxActiveViewPort> ActorFinishedSemiFlags;
	std::array<VkSemaphore, QFAWindow::MaxActiveViewPort> ActorShadowFinishedSemi;
	VkSemaphore UISemaphore;


	VkSemaphore FinisSemaphore;

	std::array<VkCommandBuffer, QFAWindow::MaxActiveViewPort> ShadowCommandBuffers;
	std::array<VkCommandBuffer, QFAWindow::MaxActiveViewPort> MeshCommandBuffers;
	VkCommandBuffer UICommandBuffer;
	VkCommandBuffer FinisCommandBuffer;
	
	/*
	ActorFinishedSemaphore  redu in DrawText
	
	*/

	static VkSemaphore imageAvailableSemaphore; 
	static VkSemaphore renderFinishedSemaphores;


	static VkSemaphore imageAvailableSemaphoreMesh;
	static VkSemaphore renderFinishedSemaphoresMesh;

public:
	QFAWindow(int width, int height, std::string name);
	~QFAWindow();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<QFAWindow*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void AddViewport(QFAViewport* viewport);
	void RemoveViewport(QFAViewport* viewport);
	QFAViewport* GetViewport(size_t index);
	inline size_t GetViewportCount()
	{
		return Viewports.Length();
	}
	inline bool ShouldClose()
	{
		return glfwWindowShouldClose(glfWindow);
	}

	inline static QFAWindow* GetMainWindow()
	{
		return MainWindow;
	}
private:
	uint32_t imageIndex; // next image in sqp shain
	
	void createCommandPool();
	
	void recreateSwapChain();

	void DrawActors(QFAViewport* viewport, bool clear);
	void recordCommandBufferMesh(QMeshBaseComponent* text, bool shadow);

	unsigned int ProcessMeshComponent(QSceneComponent* component, bool shadow);


	void DrawUI();


	void DrawOffscreenBuffer();
	void recordCommandBufferTestImege();



	void createSyncObject();
	void createCommandBuffer();

	void PresentFrame();
	

	QFAPresentImage* imugo;

	/*----*/

	// Depth bias (and slope) are used to avoid shadowing artifacts
// Constant depth bias factor (always applied)
	float depthBiasConstant = 3.05f; // 1.25f   2.05f
	// Slope depth bias factor, applied depending on polygon's slope
	float depthBiasSlope = 3.55f; // 1.75f    2.55f
	



	void StartUIRenderViewPort( int viewportIndex);
public:
	QFAVKImageView* ShadowImagesView;
	QFAVKTextureSampler* ShadowSampler;
	static VkFormat depthFormat; // VK_FORMAT_D32_SFLOAT
private:
	const float shadowResolution = 2000;
	// VkFormat
	



	QFAVKMeshFrameBuffer* frameBufferMesh; // ont only mesh

	/*---*/
	
	QFAImage* ShadowImage;
	
	

	struct SViewportBuffers
	{
		QFAVKBuffer* uiProjectionBuffer;
	};
	std::array<SViewportBuffers, MaxActiveViewPort> ViewportBuffers;

	void CreateShadow();
	void CreateViewtortsBuffers();

	FVector CurentCameraPosition;

	//in start of array number bigger
	QFAArray<QFAUIUnit*> SortUIUnits;

	void SortUIs(QFAViewportRoot* root);
	void AddUnit(QFAUIUnit* unit);

	QFAUIEvent UIEvent;
};
