#include "MainEditorWindow.h"
#include <Render/Window/Window.h>
#include <EditorUI/FileExplorer.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Canvas.h>
#include <Render/UI/Text.h>

#include <Overlord/Overlord.h>
#include <EditorUI/ExplorerFolderUnit.h>

QFAEditorMainWindow* QFAEditorMainWindow::MainWindow = nullptr;
QFAText::SFont* QFAEditorMainWindow::Icomonfont;
QFAEditorMainWindow::QFAEditorMainWindow()
{
	if (MainWindow)
		stopExecute("QFAEditorMainWindow was created")
	else
		MainWindow = this;

	QFAOverlord::SetShdowFpsInConsole(true);
	QFAOverlord::EnableFpsLock(false);
	QFAOverlord::SetLimitFpsCount(60);
	Window = new QFAWindow(800, 800, "QFAEditor");


	QFAText::SFont* font;
	if (QFAText::ELoadFontResult res = QFAText::LoadFont("SomeFont/Roboto-Regular.ttf", font))
		stopExecute(res);

	QFAText::LoadFont("SomeFont/icomoon.ttf", QFAEditorMainWindow::Icomonfont);
	CreateUI();
}

QFAEditorMainWindow::~QFAEditorMainWindow()
{
	 
}

void QFAEditorMainWindow::CreateUI()
{
	QFAViewport* mainViewPort = Window->GetViewport(0);
	WindowCanvas = new QFAUICanvas;
	mainViewPort->AddUnit(WindowCanvas);

	QFAUISlot::SCanvasSlot slot;
	slot.Width = 1.0f;
	slot.Height = 0.3f;
	slot.x = 0.0f;
	slot.y = 0.7f;
	FileExplorer = new QFAUIEditorFileExplorer(Window);
	FileExplorer->SetSlot(&slot);
	WindowCanvas->AddUnit(FileExplorer);
}

