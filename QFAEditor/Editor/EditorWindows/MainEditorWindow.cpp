﻿#include "epch.h"
#include "MainEditorWindow.h"
#include <EngineStuff/Window/EngineWindow.h>
#include <EditorUI/FileExplorer.h>
#include <EngineStuff/VulkanSuff.h>
#include <UI/Canvas.h>
#include <UI/Text.h>

#include <Overlord/Overlord.h>
#include <EditorUI/ExplorerFolderUnit.h>
#include <UI/UIList.h>
#include <Camera/CameraEditor.h>
#include <EditorFileStorage.h>
#include <Object/Actor/StaticMeshActor.h>
#include <EditorUI/GameViewportInfo.h>
#include <EngineStuff/Window/ViewportHolder.h>
#include <Tools/String.h>
#include <UI/TextInput.h>
#include <EngineClassesInterface.h>

QFAEditorMainWindow* QFAEditorMainWindow::MainWindow = nullptr;
QFAText::SFont* QFAEditorMainWindow::Icomonfont = nullptr;
QFAEditorMainWindow::QFAEditorMainWindow()
{
	if (MainWindow)
		stopExecute("QFAEditorMainWindow was created")
	else
		MainWindow = this;

	QFAOverlord::SetShdowFpsInConsole(false);
	QFAOverlord::EnableFpsLock(true);
	QFAOverlord::SetLimitFpsCount(60);
	
	Window = QFAEditorWindow::CreateEngineWindow(LoaderWidth, LoaderHeight, "QFAEditor", true, false);

	QFAText::SFont* font;
	if (QFAText::ELoadFontResult res = QFAText::LoadFont("SomeFont/Roboto-Regular.ttf", font))
		stopExecute(res);	

	QFAText::LoadFont("SomeFont/icomoon.ttf", QFAEditorMainWindow::Icomonfont);

	MainWindow->CreateLoadUI();
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
		Worlds[0]->Destroy();
		Worlds[1]->Destroy();
	}
}

void QFAEditorMainWindow::CreateMainEdirorUI()
{	
	CreateInput();
	QFAEngineViewport* mainViewPort = Window->GetViewport(0);
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
}

void QFAEditorMainWindow::PrepareGameViewport()
{
	GameViewport = (QFAEditorViewportHolder*)new QFAViewportHolder();
	Window->AddViewport(GameViewport);
	GameViewport->SetParameters(0.0f, 0.0f, 0.7f, 0.7f);

	Worlds[0] = NewObject<QWorld>();
	Worlds[1] = NewObject<QWorld>();
	Worlds[0]->SetEnable(false);
	EditorCamera = NewObject<ACameraEditor>();
	EditorCamera->SetTick(false);
	EditorCamera->SetActorPosition(FVector(-200, 0, 0));
	EditorCamera->SetActorRotation(0);
	EditorCamera->ActivateCamera(((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]);

	((QEditorWorld*)Worlds[0])->SetEditorActor(EditorCamera);
}

void QFAEditorMainWindow::PrepareCallback()
{	
	Input = new QFAInput(Window);
	Input->AddKeyPress(EKey::DELETE_KEY, "pressDelete", [this](EKey::Key key)
		{
			switch (Focus)
			{
				case QFAEditorMainWindow::FActorList:
					//GameViewportInfo->ActorList->PressDelete(); 
					break;
				case QFAEditorMainWindow::FFileExplorer:
					break;
				default:
					break;
			}
		});
}

void QFAEditorMainWindow::AddActorToWorlds(QActor* actor, std::u32string actorName, size_t id, bool isCppClass)
{
	Worlds[0]->AddActor(actor);
	GameViewportInfo->AddActor(actor, actorName, id, isCppClass);
}

void QFAEditorMainWindow::GameCompileCallback(QFAGameCode::CompileStatus status)
{	
	MainWindow->CompileStarted = false;
}

void QFAEditorMainWindow::CreateInput()
{
	
	((QFAEditorWindow*)Window)->SetDropFun([this](int path_count, const char* paths[])
		{
			if (WindowCanvas)
				FileExplorer->DropFiles(path_count, paths);
		});

	Input = new QFAInput(Window);
	Input->AddKeyRelease(EKey::MOUSE_LEFT, "lmbU", [this](EKey::Key key)
		{
			QFAEditorMainWindow::EndDragAndDrop(key);

			double x, y;
			if (Window->GetMousePosition(x, y))
			{
				FVector2D tem = PickObjectLastCursorPos - FVector2D(x, y);
				if (tem.Length() < 10.0f)
					QFAEditorMainWindow::PickMesh(key);
			}

			EditorCamera->SetTick(false);
		});

	Input->AddKeyPress(EKey::MOUSE_LEFT, "lmbD", [this](EKey::Key key)
		{
			double x, y;
			if (Window->GetMousePosition(x, y))
			{
				FVector2D pos = GameViewport->GetPosition();
				FVector2D size = GameViewport->GetSize();
				if (x >= pos.X && y >= pos.Y &&
					x <= pos.X + size.X && y <= pos.Y + size.Y)
				{
					PickObjectLastCursorPos.X = x;
					PickObjectLastCursorPos.Y = y;
					EditorCamera->SetTick(true);
				}
				else
				{
					PickObjectLastCursorPos.X = -1.0f;
					PickObjectLastCursorPos.Y = -1.0f;
				}
			}
		});

	Input->AddKeyPress(EKey::LEFT_CONTROL, "LEFT_CONTROL_down", [this](EKey::Key key)
		{
			LeftCTRLPress = true;
		});

	Input->AddKeyRelease(EKey::LEFT_CONTROL, "LEFT_CONTROL_up", [this](EKey::Key key)
		{
			LeftCTRLPress = false;
		});

	Input->AddKeyPress(EKey::B, "LEFT_CONTROL", [this](EKey::Key key)
		{
			if (LeftCTRLPress && !CompileStarted)
			{
				CompileStarted = true;
				QFAGameCode::CompileGameCode(QFAEditorMainWindow::GameCompileCallback);
			}
		});

	Input->AddKeyRelease(EKey::DELETE_KEY, "delete_release", [this](EKey::Key key)
		{
			GameViewportInfo->PressedDelete();
		});

	Input->AddKeyRelease(EKey::S, "s_release", [this](EKey::Key key)
		{
			std::cout << "before LeftCTRLPress\n";
			if (LeftCTRLPress)
			{
				if (!Level)
					Level = new QFAEditorLevel(U"Content/Level.qfa");			

				Level->SaveLevel(Worlds[0]);

				/*--- next code for test ---*/
				for (size_t i = 0; i < ((QEditorWorld*)Worlds[0])->Actors.Length(); i++)
				{					
					((QFAEditorActor*)((QEditorWorld*)Worlds[0])->Actors[i])->ActorWorld = nullptr;
					QFAEngineGameCode::GetAPI()->DeleteObject(((QEditorWorld*)Worlds[0])->Actors[i]);
				}

				/*
				
				not forget change actor list when load level
				
				*/

				Worlds[0]->DestroyWorld(false);					
				Worlds[0] = Level->GetWorld();  
				((QEditorWorld*)Worlds[0])->SetEditorActor(EditorCamera);
			}
		});

	PrepareCallback();
}

void QFAEditorMainWindow::StartDragAndDrop(bool isCppClass, size_t id)
{
	MainWindow->CurentDragId = id;
	MainWindow->IsCppClass = isCppClass;
}

void QFAEditorMainWindow::EndDragAndDrop(EKey::Key key)
{
	if (MainWindow->CurentDragId)
	{
		double viewportX = MainWindow->GameViewport->X;
		double viewportY = MainWindow->GameViewport->Y;
		double viewportWidth = MainWindow->GameViewport->Width;
		double viewportHeight = MainWindow->GameViewport->Height;
		
		int id = MainWindow->CurentDragId;
		MainWindow->CurentDragId = 0;
		double x, y;
		MainWindow->Window->GetMousePosition(x, y);
		if (x >= viewportX && y >= viewportY &&
			x <= viewportWidth && y <= viewportHeight)
		{
			if (MainWindow->IsCppClass)
			{
				if (!QFAGameCode::GetAPI())
					return;

				QFAClass* newObjectClass = (QFAGameCode::GetAPI()->GetClassList()[id]);
				if (newObjectClass->GetBaseOn() == QFAClass::ObjectClasses::Actor || 
					newObjectClass->GetBaseOn() == QFAClass::ObjectClasses::StaticMeshActor || 
					newObjectClass->GetBaseOn() == QFAClass::ObjectClasses::CameraActor)
				{
					QActor* newActor = (QActor*)QFAGameCode::GetAPI()->CreateObject(newObjectClass->GetId());
					newActor->SetActorPosition(0);
					MainWindow->AddActorToWorlds(newActor,
						QFAString::CharsTo32Chars(newObjectClass->GetName()),
						newObjectClass->GetId(), true);					
				}
				else
					std::cout << "Class not based on Actor\n";
			}
			else
			{				
				QFAEditorFileStorage::QFAContentFile& ef = QFAEditorFileStorage::GetFile(id);
				if (ef.Id == 0)
					return;
				else if (ef.fileType == QFAEditorFileStorage::QFAFileTypes::EFTMesh)
				{					 
					AStaticMeshActor* staticActor = (AStaticMeshActor*)(QFAGameCode::GetAPI()->CreateObject(QFAClass::ObjectClasses::StaticMeshActor));

					staticActor->GetClass()->GetId();



					staticActor->SetActorPosition(0);
					staticActor->SetMesh((QFAMeshData*)ef.file);
					MainWindow->AddActorToWorlds(staticActor, 
						std::filesystem::path(ef.path).filename().replace_extension("").u32string(),
						ef.Id, false);
				}
			}			
		}		
	}
}

void QFAEditorMainWindow::PickMesh(EKey::Key key)
{
	Window->GetMeshUnderCursore([this](QMeshBaseComponent* mesh)
		{
			GameViewportInfo->SelectActor(mesh->GetActor());
		});
}

void QFAEditorMainWindow::CreateLoadUI()
{
	QFAEngineViewport* mainViewPort = ((QFAEditorWindow*)Window)->Viewports[0];
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

	//std::cout << mainViewPort << "\n";
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