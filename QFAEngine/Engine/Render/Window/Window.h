#pragma once
#include <Tools/Array.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>
#include <string>

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
	static QFAWindow* MainWindow;
	GLFWwindow* glfWindow;
	static bool Init;
	int Width;
	int Height;
	int NewHeight;
	int NewWidth;
	bool WindowSizeChanched = false;

	int countFarame;
	double acumulateDeltatime;

	

	QFAArray<QFAViewport*> Viewports;

	void StartFrame();
	void EndFrame();
	
	static void RenderWindow();	
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
		return glfwWindowShouldClose(glfWindow);
	}

	inline static QFAWindow* GetMainWindow()
	{
		return MainWindow;
	}
private:

};
