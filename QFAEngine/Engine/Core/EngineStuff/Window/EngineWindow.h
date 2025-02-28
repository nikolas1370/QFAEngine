#pragma once
#include <Tools/Array.h>
#include <Core/EngineStuff/VulkanSuff.h>
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
class QFAMeshData;
class QFAVKSwapChain;
class QFAUIEvent;
class QFAEngineViewport;
class QFAPresentImage;
class QFAViewportRoot;
class QFAVKTextRenderPass;
class QFAVKShadowFrameBuffer;
class QFAVKImageView;
class QFAVKTextureSampler;
class QFAVKBuffer;
class QFAVKRenderPassDepth;
class QFAVKInstance;
class QFAViewport;

struct QFAWindowParameter
{
	int width;
	int height;
	std::string name;
	bool inCenter = false;
	bool decorated = true;
	std::function<void()> closedFun = nullptr;
};

class QFAEXPORT QFAEngineWindow
{
	friend QFAText;; 
	friend QFAUIImage;
	friend QFAOverlord;
	friend QMeshBaseComponent;	
	friend QFAInput;
	friend QFAVKSwapChain;
	friend QFAImage;
	friend QWorld;
	friend QFAMeshData;
	friend QFAViewport;
	friend QFAEngineViewport;
	friend QFAUIEvent;

	static void* operator new[](size_t) = delete;
	static void  operator delete  (void*) = delete;
	static void  operator delete[](void*) = delete;

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

	struct CharCallback
	{
		QFAEngineWindow* ChileWindow;
		// in QFAUIEvent::CharCallback call it
		std::function<void(GLFWwindow*, unsigned int)> callback;
	};

	const float shadowResolution = 2000;

	static std::vector<QFAEngineWindow*> Windows;

	static QFAEngineWindow* CurentProcessWindow;
	static QFAVKInstance* Instance;
	static QFAVKRenderPassSwapChain* RenderPassSwapChain;
	static QFAVKRenderPassDepth* RenderPassOffScreen;
	static QFAVKTextRenderPass* TextRenderPass;
	static QFAVKTextRenderPass* TextRenderPassClear;
	static int ViewportProcess;
	// all windows use same ShadowFrameBuffer
	static QFAVKShadowFrameBuffer* ShadowFrameBuffer;
	static QFAVKImageView* ShadowImagesView;
	static QFAVKTextureSampler* ShadowSampler;
	static VkFormat depthFormat; // VK_FORMAT_D32_SFLOAT
	/*
	this use only for viewports in windows.
	In process of drawing when execute RenderWindow
*/
	static std::vector<SViewportStuff> ViewportStuff;
	static VkCommandBuffer FinisCommandBuffer;
	static VkSemaphore* NextWaitSemi;
	static QFAImage* ShadowImage;
	static VkCommandPool commandPool;
	
	GLFWwindow* glfWindow;
	int Width;
	int Height;
	int NewHeight;
	int NewWidth;
	bool WindowSizeChanched = false;
	VkSurfaceKHR surface;
	QFAVKSwapChain* SwapChain;

	
	
	VkSemaphore FinisSemaphore;
	VkSemaphore FinisSemiOtherWindow;
	VkSemaphore GetImageSemaphore;
	
	bool NeedClose = false;
	// call if window closed and delete
	std::function<void()> ClosedFun;

	std::function<void(int path_count, const char* paths[])> DropFunction;

	uint32_t imageIndex; // next image in sqp shain
	bool minimized = false;
	QFAPresentImage* PresentImage;

	// Depth bias (and slope) are used to avoid shadowing artifacts
// Constant depth bias factor (always applied)
	float depthBiasConstant = 3.05f; // 1.25f   2.05f
	// Slope depth bias factor, applied depending on polygon's slope
	float depthBiasSlope = 3.55f; // 1.75f    2.55f

	// VkFormat
	QFAVKMeshFrameBuffer* frameBuffer;
	FVector CurentCameraPosition;
	//in start of array number bigger
	QFAArray<QFAUIUnit*> SortUIUnits;
	bool CursorOnWindow = false;
	QFAUIEvent* UIEvent;
	VkCommandBuffer CurentComandBuffer = nullptr;
	QFAVKBuffer* PickBuffer;
	std::function<void(QMeshBaseComponent*)> GetMeshCallback;

protected:
	std::vector<QFAEngineViewport*> Viewports;
	// if window inside viewportHolder valude be false
	bool RegularWindow = true;
	QFAEngineWindow* ParentWindow = nullptr; // if windw not regular this be not 
	std::vector<CharCallback> WindowChildCallback; // list not regular window
private:
	static void QueueSubmitPresent(std::vector<VkSemaphore>& listSemi);
	// call after QFAEngineWindow::CheckIfNeedResizeWindows
	static void ProcessGetMeshId();
	static void PresentWindows();
	// call Overloed
	static void ProcessUIEvent();
	void ProcessUIEventInside();
	// use after all render and present done, every frame
	static void CheckIfNeedResizeWindows();
	// call Overlord
	static void RenderWindows();



private:
	void StartFrame();
	void RenderWindow(bool lastWindow);
	void RenderViewport(QFAEngineViewport* viewport, size_t i, bool& clear);
	void BeginCommandBuffer();
	void EndCommandBufferAndQueueSubmit();
	void ShadowRender(QFAEngineViewport* viewport);
	void CreateViewPortStuff();
	void createCommandPool();
	void recreateSwapChain();
	void DrawActors(QFAEngineViewport* viewport, bool clear);
	unsigned int ProcessMeshComponent(QSceneComponent* component, bool shadow);
	void DrawUI(QFAEngineViewport* viewport, bool clear);
	void DrawOffscreenBuffer();
	void recordCommandBufferTestImege();
	void createSyncObject();
	void createCommandBuffer();
	void PresentFrame(VkSemaphore finishSemi);
	//void StartUIRenderViewPort(int viewportIndex);
	void StartUIRenderViewPort(QFAEngineViewport* viewport);
	void CreateShadow();
	void SortUIs(QFAViewportRoot* root);
	void AddUnit(QFAUIUnit* unit);	
	

	void AddChildWindow(QFAEngineWindow* win);
	void RemoveChildWindow(QFAEngineWindow* childWin);
	void AddCharCallback(QFAEngineWindow* childWin, std::function<void (GLFWwindow*, unsigned int)> callback);
	

	QFAEngineWindow(int width, int height, std::string name, bool inCenter = false, bool decorated = true, std::function<void()> closedFun = nullptr);
protected:
	// if this not regular window
	QFAEngineWindow();	

	void SetClosedFun(std::function<void()> fun);

	void InitNotRegularWindow(QFAEngineWindow* parentWidow);
	// if need delete window use Close()
	virtual ~QFAEngineWindow();

	inline void SetDropFun(std::function<void(int path_count, const char* paths[])> fun)
	{
		DropFunction = fun;
	}
	bool ShouldClose();

	static QFAEngineWindow* CreateEngineWindow(const int width, const int height, const std::string name, const bool inCenter = false, const bool decorated = true, const std::function<void()> closedFun = nullptr)
	{
		QFAEngineWindow* win = (QFAEngineWindow*)malloc(sizeof(QFAEngineWindow));
		return new (win) QFAEngineWindow(width, height, name, inCenter, decorated, closedFun);
	}
public:



	void AddViewport(QFAViewport* viewport);
	// can't delete last viewport
	void RemoveViewport(QFAViewport* viewport);
	QFAViewport* GetViewport(size_t index);
	inline size_t GetViewportCount()
	{
		return Viewports.size();
	}

	// close and delete in next frame
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

	void GetFocus();
	void EnabelDecorated(bool enabel);
	void SetSize(int w, int h);		
	void MoveToCenter();
	/*
		
		need test in game window
	
	*/
	void GetMeshUnderCursore(std::function<void(QMeshBaseComponent*)> callback);

	void SetWindowTitle(const char* winName);

	static QFAEngineWindow* GetMainWindows();
};
