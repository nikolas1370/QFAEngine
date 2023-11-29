#include "Window.h"
#include <Render/Buffer/FramebufferMain.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Render/Buffer/Framebuffer.h>
#include <Render/Time.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Overlord/Overlord.h>
#include <Render/Window/Viewport.h>


QFAWindow* QFAWindow::MainWindow = nullptr;

bool QFAWindow::Init = false;

QFAWindow::QFAWindow(int width, int height, std::string name)
{
	if (!QFAWindow::MainWindow)
		QFAWindow::MainWindow = this;

	Width = width;
	Height = height;
	glfWindow = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!glfWindow)
    {
        glfwTerminate();
        ASSERT(false);
        return;
    }

    glfwMakeContextCurrent(glfWindow);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) 	// before glfwMakeContextCurrent
	{
		glfwTerminate();
		ASSERT(false);
		return ;
	}

	QFAFrameBufferMain::Init(width, height);
	QFAViewport* vp = new QFAViewport;
	Viewports.Add(vp);
	vp->Settup( width, height);
	
	if (!QFAWindow::Init)
	{		
		QFAWindow::Init = true;
		glfwSetWindowSizeCallback(glfWindow, [](GLFWwindow* win, int w, int h)
		{			
				
			if (QFAWindow::GetMainWindow()->glfWindow == win)
			{
				// check if 0
				QFAWindow::GetMainWindow()->NewWidth = w;
				QFAWindow::GetMainWindow()->NewHeight = h;
				QFAWindow::GetMainWindow()->WindowSizeChanched = true;
			}		
		});
	}
}

QFAWindow::~QFAWindow()
{
    glfwDestroyWindow(glfWindow);
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
	uint64_t sft = QTime::GetWorkTime();
	if (WindowSizeChanched)
	{
		Width = NewWidth;
		Height = NewHeight;		
		WindowSizeChanched = false;
		for (int j = 0; j < Viewports.Length(); j++)
			Viewports[j]->Settup(Width, Height);
	}
	
	for (int i = 0; i < Viewports.Length(); i++)
		Viewports[i]->ProcessFrame(sft);

	EndFrame();
}

void QFAWindow::RenderWindow()
{
	MainWindow->StartFrame();
}

void QFAWindow::EndFrame()
{
	for (int i = 0; i < Viewports.Length(); i++)
		QFAFrameBufferMain::CopyFrameBuffer(Viewports[i]);

	glfwSwapBuffers(glfWindow);
	countFarame++;
	acumulateDeltatime += QTime::GetDeltaTime();
	if (acumulateDeltatime >= 1.0)
	{
	//	std::cout << "Frame time " << acumulateDeltatime / countFarame << " count " << countFarame << "\n";
		countFarame = 0;
		acumulateDeltatime = 0.0;
	}
}


