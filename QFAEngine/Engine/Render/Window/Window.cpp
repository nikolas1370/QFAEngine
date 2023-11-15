#include "Window.h"
#include <Render/Buffer/FramebufferMain.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Buffer/Framebuffer.h>
#include <Render/Time.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Overlord/Overlord.h>
#include <Render/Window/Viewport.h>

QFAArray<QFAWindow*> QFAWindow::Windows;
QFAWindow* QFAWindow::MainWindow = nullptr;

bool QFAWindow::Init = false;

QFAWindow::QFAWindow(int width, int height, std::string name)
{
	if (!QFAWindow::MainWindow)
		QFAWindow::MainWindow = this;

	Width = width;
	Height = height;
	Window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!Window)	
    {
        glfwTerminate();
        ASSERT(false);
        return;
    }

    glfwMakeContextCurrent(Window);

	if (glewInit() != GLEW_OK) 	// before glfwMakeContextCurrent
	{
		glfwTerminate();
		ASSERT(false);
		return ;
	}

    QFAWindow::Windows.Add(this);

	/* be until not be support multi windows */
	QFAFrameBufferMain::Init(width, height);
	QFAViewport* vp = new QFAViewport;
	Viewports.Add(vp);
	vp->Settup( width, height);
	
	if (!QFAWindow::Init)
	{		
		QFAWindow::Init = true;
		glfwSetWindowSizeCallback(Window, [](GLFWwindow* win, int w, int h)
		{
			for (int i = 0; i < Windows.Length(); i++)
			{
				if (Windows[i]->Window == win)
				{
					// check if 0
					Windows[i]->NewWidth = w;
					Windows[i]->NewHeight = h;
					Windows[i]->WindowSizeChanched = true;			
					break;
				}

			}
		});
	}
}

QFAWindow::~QFAWindow()
{
    glfwDestroyWindow(Window);    
    QFAWindow::Windows.Remove(this);
}

void QFAWindow::AddViewport(QFAViewport* viewport)
{
	Viewports.Add(viewport);
	viewport->Settup(Width, Height);
}

QFAViewport* QFAWindow::GetViewport(size_t index)
{
	if (index < 0 || index >= Viewports.Length())
		return nullptr;
	else
		return Viewports[index];		
}

void QFAWindow::StartFrame()
{	
	if (WindowSizeChanched)
	{
		Width = NewWidth;
		Height = NewHeight;		
		WindowSizeChanched = false;
		for (int j = 0; j < Viewports.Length(); j++)
			Viewports[j]->Settup(Width, Height);
	}
	
	for (int i = 0; i < Viewports.Length(); i++)
		Viewports[i]->ProcessFrame();

	EndFrame();
}

void QFAWindow::RenderWindows()
{
	for (int i = 0; i < QFAWindow::Windows.Length(); i++)
	{
		QFAWindow::Windows[i]->StartFrame();
	}	
}

void QFAWindow::EndFrame()
{
	for (int i = 0; i < Viewports.Length(); i++)
		QFAFrameBufferMain::CopyFrameBuffer(Viewports[i]);

	glfwSwapBuffers(Window);
	countFarame++;
	acumulateDeltatime += QTime::GetDeltaTime();
	if (acumulateDeltatime >= 1.0)
	{
		countFarame = 0;
		acumulateDeltatime = 0.0;
	}
}


