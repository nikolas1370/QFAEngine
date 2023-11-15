#pragma once
#include <Tools/Array.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>

struct GLFWwindow;
class QFAViewport;
class QFAOverlord;
class QFAFrameBufferMain;
class QFAFrameBuffer;
class QMeshBaseComponent;
class QCameraComponent;
class QFAWindow
{
	friend QFAOverlord;
	//friend QFAWindow;
	static QFAArray<QFAWindow*> Windows;
	static QFAWindow* MainWindow;
	GLFWwindow* Window;
	static bool Init;
	int Width;
	int Height;
	int NewHeight;
	int NewWidth;
	bool WindowSizeChanched = false;

	int countFarame;
	double acumulateDeltatime;
	//glm::mat4 MatrixPerspective;
	
	

	QFAArray<QFAViewport*> Viewports;

	void StartFrame();
	void EndFrame();
	
	static void RenderWindows();	
public:
	QFAWindow(int width, int height, std::string name);
	~QFAWindow();

	void AddViewport(QFAViewport* viewport);
	QFAViewport* GetViewport(size_t index);
	inline size_t GetViewportCount()
	{
		return Viewports.Length();
	}
	inline bool ShouldClose()
	{
		return glfwWindowShouldClose(Window);
	}

	inline static QFAWindow* GetMainWindow()
	{
		return MainWindow;
	}
private:

};
