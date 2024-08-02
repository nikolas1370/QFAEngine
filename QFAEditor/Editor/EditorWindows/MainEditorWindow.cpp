#include "epch.h"
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
	WindowCanvas->SetWidth("100%");
	WindowCanvas->SetHeight("100%");
	mainViewPort->AddUnit(WindowCanvas);

	TopInWindowCanvas = new QFAUICanvas;	
	WindowCanvas->AddUnit(TopInWindowCanvas);
	TopInWindowCanvas->SetWidth("100%");
	TopInWindowCanvas->SetHeight("30");
	TopInWindowCanvas->SetBackgroundColor(QFAColor(36, 36, 36));

	RunButton = new QFAText;
	TopInWindowCanvas->AddUnit(RunButton);
	RunButton->SetTextSize(25);
	RunButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	RunButton->SetText(PlaySymbol);
	RunButton->SetWidth("25");
	RunButton->SetHeight("25");
	SetRunButton();

	FileExplorer = new QFAUIEditorFileExplorer(Window, QFAEditorMainWindow::StartDragAndDrop);	
	FileExplorer->SetWidth("100%");
	FileExplorer->SetHeight("30%");	
	FileExplorer->SetTop("70%");
	WindowCanvas->AddUnit(FileExplorer);
	
	GameViewportInfo = new QFAEditorGameViewportInfo;	
	GameViewportInfo->SetWidth("30%");
	GameViewportInfo->SetHeight("70%");
	GameViewportInfo->SetLeft("70%");
	GameViewportInfo->SetTop("30");
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
	GameViewport->SetTop("30");
	GameViewport->SetLeft("0");
	GameViewport->SetHeight("70% - 30");
	GameViewport->SetWidth("70%");
	
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

void QFAEditorMainWindow::SetRunButton()
{
	RunButton->Events.SetLeftMouseDownUp([this](QFAUIUnit* unit)
	{		
		if (QFAGameCode::CompileInWork)
			return;
		
		if (QFAEngineViewport::GetInGame())
			EndGame(); 
		else
			StartGame();

		QFAEngineViewport::SetInGame(!QFAEngineViewport::GetInGame());
	});
}

void QFAEditorMainWindow::StartGame()
{
	RunButton->SetText(StopSymbol);
	((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]->ChangeCamera(nullptr);
	GameViewport->ChangeWindow();
	for (size_t i = 0; i < ((QEditorWorld*)Worlds[0])->Actors.Length(); i++)
	{		
		QObject* object = QFAGameCode::GetAPI()->CreateObject(
			((QEditorWorld*)Worlds[0])->Actors[i]->GetClass()->GetId());

		if (object)
			Worlds[1]->AddActor((QActor*)object);
	}	
}

void QFAEditorMainWindow::EndGame()
{
	RunButton->SetText(PlaySymbol);
	((QEditorWorld*)Worlds[1])->DestroyWorld(true);

	Worlds[1] = NewObject<QWorld>();
	GameViewport->ChangeWindow();
	EditorCamera->ActivateCamera(((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]);
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
			if (!QFAEngineViewport::GetInGame() && LeftCTRLPress && !QFAGameCode::CompileInWork)
				QFAGameCode::CompileGameCode([](QFAGameCode::CompileStatus status) {});
		});

	Input->AddKeyRelease(EKey::DELETE_KEY, "delete_release", [this](EKey::Key key)
		{
			GameViewportInfo->PressedDelete();
		});

	Input->AddKeyRelease(EKey::S, "s_release", [this](EKey::Key key)
		{
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
	if (QFAEngineViewport::GetInGame())
		return;

	MainWindow->CurentDragId = id;
	MainWindow->IsCppClass = isCppClass;
}

void QFAEditorMainWindow::EndDragAndDrop(EKey::Key key)
{
	if (!QFAEngineViewport::GetInGame() && MainWindow->CurentDragId)
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
	TextList->SetTop("30");

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