#pragma once
#include <Tools/Array.h>
#include <Tools/VulkanSuff.h>
#include <string>
#include <Math/Vector.h>


#include <functional>
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
class QFAInput;
class MeshData;
class QFAVKSwapChain;
class QFAUIEvent;
class QFAViewport;
class QFAPresentImage;
class QFAViewportRoot;
class QFAVKTextRenderPass;
class QFAVKShadowFrameBuffer;
class QFAVKImageView;
class QFAVKTextureSampler;
class QFAVKBuffer;
class QFAVKRenderPassDepth;
class QFAVKInstance;
class QFAEXPORT QFAWindow
{

	friend QFAText;; 
	friend QFAUIImage;
	friend QFAOverlord;
	friend QMeshBaseComponent;	
	friend QFAInput;
	friend QFAVKSwapChain;
	friend QFAImage;
	friend QWorld;
	friend MeshData;

	static std::vector<QFAWindow*> Windows;
	static QFAWindow* CurentProcessWindow;
	GLFWwindow* glfWindow;
	

	int Width;
	int Height;
	int NewHeight;
	int NewWidth;
	bool WindowSizeChanched = false;
	static QFAVKInstance* Instance;
	VkSurfaceKHR surface;	
	QFAVKSwapChain *SwapChain;


	static QFAVKRenderPassSwapChain* RenderPassSwapChain;
	static QFAVKRenderPassDepth* RenderPassOffScreen;
	static QFAVKTextRenderPass* TextRenderPass;
	static QFAVKTextRenderPass* TextRenderPassClear;



	QFAArray<QFAViewport*> Viewports;

	// call Overloed
	static void ProcessUIEvent();
	// use after all render and present done
	static void CheckIfNeedResizeWindows();
	void StartFrame();
	

	// call Overlord
	static void RenderWindows();	
	void RenderWindow(bool lastWindow);
	static void PresentWindows();
public:

	 
	
private:
	static int ViewportProcess;
	// all windows use same ShadowFrameBuffer
	static QFAVKShadowFrameBuffer* ShadowFrameBuffer;
	static QFAVKImageView* ShadowImagesView;
	static QFAVKTextureSampler* ShadowSampler;
	static VkFormat depthFormat; // VK_FORMAT_D32_SFLOAT


	void BeginCommandBuffer();
	void EndCommandBufferAndQueueSubmit();
	void ShadowRender(QFAViewport* viewport);

	/*----*/

	static VkCommandPool commandPool;

	struct SViewportSemi
	{
		VkSemaphore Actor;
		VkSemaphore ActorShadow;
		VkSemaphore Ui;
	};


	struct SViewportBuffers
	{   // this buffer attach to text/image pipline sets0 
		QFAVKBuffer* uiProjectionBuffer;
		// this buffer attach to mesh pipline sets0 
		QFAVKBuffer* worldProjectionBuffer;
		QFAVKBuffer* shadowBuffer;
	};

	struct SViewportStuff
	{
		SViewportSemi semaphore;
		VkCommandBuffer comandBuffer;
		SViewportBuffers buffers;
	};

	/*
		this use only for viewports in windows.
		In process of drawing when execute RenderWindow
	*/
	static std::vector<SViewportStuff> ViewportStuff;
	VkSemaphore FinisSemaphore;
	VkSemaphore FinisSemiOtherWindow;
	VkSemaphore GetImageSemaphore;
	static VkCommandBuffer FinisCommandBuffer;

	void CreateViewPortStuff();

	bool NeedClose = false;
	// call if window closed and delete
	std::function<void()> ClosedFun;
public:
	QFAWindow(int width, int height, std::string name, bool inCenter = false, bool decorated = true, std::function<void ()> closedFun = nullptr);
	~QFAWindow();


	void AddViewport(QFAViewport* viewport);
	void RemoveViewport(QFAViewport* viewport);
	QFAViewport* GetViewport(size_t index);
	inline size_t GetViewportCount()
	{
		return Viewports.Length();
	}
	bool ShouldClose();

	static QFAWindow* GetMainWindow();
	

	inline void Close()
	{
		NeedClose = true;
	}

	/*
		returns position of cursor, in window coordinates,
		relative to the upper-left corner of the content 
		area of the specified window.		

		return true if mouse on window
		return false if mouse out window
	*/
	bool GetMousePosition(double& x, double& y);

	inline void SetProcessTickIfWindowsMinimized(bool processTickIfWindowsMinimized)
	{
		ProcessTickIfWindowsMinimized = processTickIfWindowsMinimized;
	}

	

	inline void SetDropFun(std::function<void (int path_count, const char* paths[])> fun)
	{
		DropFunction = fun;
	}

	void GetFocus();
	void EnabelDecorated(bool enabel);
	void SetSize(int w, int h);		
	void MoveToCenter();

private:
	std::function<void(int path_count, const char* paths[])> DropFunction;

	uint32_t imageIndex; // next image in sqp shain
	bool minimized = false;
	bool ProcessTickIfWindowsMinimized = false;

	void createCommandPool();
	
	void recreateSwapChain();

	void DrawActors(QFAViewport* viewport, bool clear, int viewportIndex);

	unsigned int ProcessMeshComponent(QSceneComponent* component, bool shadow);


	void DrawUI(QFAViewport* viewport, int viewportIndex, bool clear);


	void DrawOffscreenBuffer();
	void recordCommandBufferTestImege();

	static void QueueSubmitPresent(std::vector<VkSemaphore> &listSemi);

	void createSyncObject();
	void createCommandBuffer();

	void PresentFrame(VkSemaphore finishSemi);
	

	QFAPresentImage* PresentImage;

	/*----*/

	// Depth bias (and slope) are used to avoid shadowing artifacts
// Constant depth bias factor (always applied)
	float depthBiasConstant = 3.05f; // 1.25f   2.05f
	// Slope depth bias factor, applied depending on polygon's slope
	float depthBiasSlope = 3.55f; // 1.75f    2.55f
	



	void StartUIRenderViewPort(int viewportIndex);
public:

	void GetMeshUnderCursore(std::function<void(QMeshBaseComponent*)> callback);
private:
	const float shadowResolution = 2000;
	// VkFormat
	



	QFAVKMeshFrameBuffer* frameBuffer; 

	/*---*/
	
	static QFAImage* ShadowImage;
	
	

	

	void CreateShadow();


	FVector CurentCameraPosition;

	//in start of array number bigger
	QFAArray<QFAUIUnit*> SortUIUnits;

	void SortUIs(QFAViewportRoot* root);
	void AddUnit(QFAUIUnit* unit);

	QFAUIEvent *UIEvent;

	bool CursorOnWindow = false;


	VkCommandBuffer CurentComandBuffer = nullptr;

	static VkSemaphore* NextWaitSemi;


	QFAVKBuffer* PickBuffer;

	
	std::function<void(QMeshBaseComponent*)> GetMeshCallback;
	// call after QFAWindow::CheckIfNeedResizeWindows
	static void ProcessGetMeshId();
};
