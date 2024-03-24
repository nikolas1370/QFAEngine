#include "MainEditorWindow.h"
#include <Render/Window/Window.h>
#include <EditorUI/FileExplorer.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Canvas.h>
#include <Render/UI/Text.h>

#include <Overlord/Overlord.h>
#include <EditorUI/ExplorerFolderUnit.h>
#include <Render/UI/UIList.h>
#include <Camera/CameraEditor.h>
#include <EditorFileStorage.h>
#include <Object/Actor/StaticMeshActor.h>
#include <EditorUI/GameViewportInfo.h>
#include <EditorUI/UIActorList.h>

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

	Input = new QFAInput(Window);
	Input->AddKeyRelease(EKey::MOUSE_LEFT, "lmbU", QFAEditorMainWindow::EndDragAndDrop);
}

QFAEditorMainWindow::~QFAEditorMainWindow()
{

	delete LoadCanvas;
	delete Text;
	delete LoadText;

	if (WindowCanvas)
	{
		delete WindowCanvas;
		delete FileExplorer;
		delete[]Worlds;
	}
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
	FileExplorer = new QFAUIEditorFileExplorer(Window, QFAEditorMainWindow::StartDragAndDrop);
	FileExplorer->SetSlot(&slot);
	WindowCanvas->AddUnit(FileExplorer);
	mainViewPort->AddUnit(WindowCanvas);
	
	
	slot.Width = 0.3f;
	slot.Height = 0.7f;
	slot.x = 0.7f;
	slot.y = 0.0f;
	GameViewportInfo = new QFAEditorGameViewportInfo;	
	GameViewportInfo->SetSlot(&slot);
	WindowCanvas->AddUnit(GameViewportInfo);
	GameViewportInfo->SetBackgroundColor(QFAColor(36, 36, 36));
	
	Window->SetSize(WorkWidth, WorkHeight);
	Window->MoveToCenter();
	Window->EnabelDecorated(true);	
	PrepareGameViewport();
	PrepareCallback();	
}

void QFAEditorMainWindow::PrepareGameViewport()
{
	GameViewport = new QFAViewport();
	Window->AddViewport(GameViewport);
	GameViewport->SetParameters(0.0f, 0.0f, 0.7f, 0.7f);

	Worlds = new QWorld[2];
	Worlds[0].SetEnable(false);
	EditorCamera = new ACameraEditor;
	EditorCamera->SetActorPosition(FVector(-200, 0, 0));
	EditorCamera->SetActorRotation(0);
	EditorCamera->ActivateCamera(GameViewport);

	Worlds[0].SetEditorActor(EditorCamera);
}

void QFAEditorMainWindow::PrepareCallback()
{
	
	Input = new QFAInput(Window);
	Input->AddKeyPress(EKey::DELETE, "pressDelete", [this](EKey::Key key)
		{
			switch (Focus)
			{
				case QFAEditorMainWindow::FActorList:
					GameViewportInfo->ActorList->PressDelete(); break;
				case QFAEditorMainWindow::FFileExplorer:
					break;
				default:
					break;
			}
		});

	GameViewportInfo->ActorList->SelectFun = [this]()
		{
			Focus = EFocus::FActorList;
		};
}

void QFAEditorMainWindow::AddActorToWorlds(QActor* actor, SEditorFile& ef)
{
	Worlds[0].AddActor(actor);	
	GameViewportInfo->ActorList->AddActor(actor, ef);
}

void QFAEditorMainWindow::StartDragAndDrop(size_t fileId)
{
	MainWindow->CurentDragFileId = fileId;
}

void QFAEditorMainWindow::EndDragAndDrop(EKey::Key key)
{
	if (MainWindow->CurentDragFileId)
	{
		double viewportX = MainWindow->GameViewport->X;
		double viewportY = MainWindow->GameViewport->Y;
		double viewportWidth = MainWindow->GameViewport->Width;
		double viewportHeight = MainWindow->GameViewport->Height;
		
		size_t id = MainWindow->CurentDragFileId;
		MainWindow->CurentDragFileId = 0;

		double x, y;
		MainWindow->Window->GetMousePosition(x, y);
		if (x >= viewportX && y >= viewportY &&
			x <= viewportWidth && y <= viewportHeight)
		{
			SEditorFile ef = QFAEditorFileStorage::GetFile(id);
			if (ef.id == 0)
				return;
			else if(ef.fileType == QFAEditorFileTypes::EFTMesh)
			{
				AStaticMeshActor* staticActor = new AStaticMeshActor;
				staticActor->SetActorPosition(0);
				staticActor->SetMesh((MeshData*)ef.file);
				MainWindow->AddActorToWorlds(staticActor, ef);
			}			
		}		
	}
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

