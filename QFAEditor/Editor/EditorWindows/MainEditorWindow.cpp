#include "MainEditorWindow.h"
#include <Render/Window/Window.h>
#include <EditorUI/FileExplorer.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Canvas.h>
#include <Render/UI/Text.h>

#include <Overlord/Overlord.h>
#include <EditorUI/ExplorerFolderUnit.h>
#include <Render/UI/UIList.h>



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
	Window = new QFAWindow(LoaderWidth, LoaderHeight, "QFAEditor", true, false);


	QFAText::SFont* font;
	if (QFAText::ELoadFontResult res = QFAText::LoadFont("SomeFont/Roboto-Regular.ttf", font))
		stopExecute(res);	

	QFAText::LoadFont("SomeFont/icomoon.ttf", QFAEditorMainWindow::Icomonfont);

	Window->SetDropFun([this](int path_count, const char* paths[])
		{
			if(WindowCanvas)
				FileExplorer->DropFiles(path_count, paths);
		});	

	MainWindow->CreateLoadUI();
}

QFAEditorMainWindow::~QFAEditorMainWindow()
{
	delete WindowCanvas;
	delete FileExplorer;
	delete LoadCanvas;
	delete Text;
	delete LoadText;
}

void QFAEditorMainWindow::CreateUI()
{	
	QFAViewport* mainViewPort = Window->GetViewport(0);
	mainViewPort->GetRoot()->removeAllUnit();

	WindowCanvas = new QFAUICanvas;	
	QFAUISlot::SCanvasSlot slot;
	slot.Width = 1.0f;
	slot.Height = 0.3f;
	slot.x = 0.0f;
	slot.y = 0.7f;
	FileExplorer = new QFAUIEditorFileExplorer(Window);
	FileExplorer->SetSlot(&slot);
	WindowCanvas->AddUnit(FileExplorer);
	mainViewPort->AddUnit(WindowCanvas);


	Window->SetSize(WorkWidth, WorkHeight);
	Window->MoveToCenter();
	Window->EnabelDecorated(true);
}

void QFAEditorMainWindow::CreateLoadUI()
{
	QFAViewport* mainViewPort = Window->GetViewport(0);
	QFAUISlot::SCanvasSlot slot;
	LoadCanvas = new QFAUICanvas;
	TextList = new QFAUIList;
	TextList->SetListType(QFAUIList::LTHorizonInner);
	Text = new QFAText;
	LoadText = new QFAText;
	LoadText_2 = new QFAText;

	LoadCanvas->AddUnit(TextList);
	LoadCanvas->AddUnit(Text);
	TextList->AddUnit(LoadText);
	TextList->AddUnit(LoadText_2);
	
	Text->SetTextSize(30);
	LoadText->SetTextSize(20);
	LoadText_2->SetTextSize(20);
	LoadText->SetOverflowWrap(QFAText::OWNone);
	LoadText_2->SetOverflowWrap(QFAText::OWNone);

	Text->SetText(U"QFAEditor");

	slot.xInPixel = true;
	slot.yInPixel = true;
	Text->SetSlot(&slot);

	slot.y = 30.0f;
	TextList->SetSlot(&slot);

	mainViewPort->AddUnit(LoadCanvas);
}

void QFAEditorMainWindow::ChangeLoadInfo(std::u32string text, std::u32string text_2)
{
	if (!WindowCanvas)
	{
		LoadText->SetText(text);
		LoadText_2->SetText(text_2);
	}
}

void QFAEditorMainWindow::DropFun(int path_count, const char* paths[])
{

}