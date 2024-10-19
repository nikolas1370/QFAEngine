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

#include <unicode/unistr.h>
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
	LoadCanvas->Destroy();
	Text->Destroy();
	LoadText->Destroy();

	if (WindowCanvas)
	{
		WindowCanvas->Destroy();
		FileExplorer->Destroy();
		Worlds[0]->Destroy();
		Worlds[1]->Destroy();
	}
}

void QFAEditorMainWindow::CreateMainEdirorUI()
{	
	CreateInput();
	QFAEngineViewport* mainViewPort = Window->GetViewport(0);
	mainViewPort->GetRoot()->removeAllUnit();

	WindowCanvas = NewUI<QFAUICanvas>();
	WindowCanvas->SetWidth("100%");
	WindowCanvas->SetHeight("100%");
	mainViewPort->AddUnit(WindowCanvas);

	TopInWindowCanvas = NewUI<QFAUICanvas>();
	WindowCanvas->AddUnit(TopInWindowCanvas);
	TopInWindowCanvas->SetWidth("100%");
	TopInWindowCanvas->SetHeight("30");
	TopInWindowCanvas->SetBackgroundColor(QFAColor(36, 36, 36));

	RunButton = NewUI<QFAText>();
	TopInWindowCanvas->AddUnit(RunButton);
	RunButton->SetTextSize(25);
	RunButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	RunButton->SetText(PlaySymbol);
	RunButton->SetWidth("25");
	RunButton->SetHeight("25");
	SetRunButton();

	FileExplorer = NewUI<QFAUIEditorFileExplorer>(Window, QFAEditorMainWindow::StartDragAndDrop,
		[this](QFAEditorFileStorage::QFAContentFile& cf) {LoadLevel(cf); });

	FileExplorer->SetWidth("100%");
	FileExplorer->SetHeight("30%");	
	FileExplorer->SetTop("70%");
	WindowCanvas->AddUnit(FileExplorer);
	
	GameViewportInfo = NewUI<QFAEditorGameViewportInfo>();
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
	((QEditorWorld*)Worlds[0])->SilenceWorld = true;
	EditorCamera = NewObject<ACameraEditor>();	
	EditorCamera->SetTick(false);
	EditorCamera->SetActorPosition(FVector(-200, 0, 0));
	EditorCamera->SetActorRotation(0);
	EditorCamera->ActivateCamera(((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]);
	((QEditorWorld*)Worlds[0])->SetEditorActor(EditorCamera);		 
	QEditorCameraComponent::ZerosMainCamera();
	CurentWorld = Worlds[0];
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
		
		bool inGame = QFAEngineViewport::GetInGame();
		QFAEngineViewport::SetInGame(!QFAEngineViewport::GetInGame());
		if (inGame)
			EndGame();
		else
			StartGame();
	});
}

void QFAEditorMainWindow::StartGame()
{
	QEditorCameraComponent::ZerosMainCamera();
	RunButton->SetText(StopSymbol);
	((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]->ChangeCamera(nullptr);
	GameViewport->ChangeWindow();	
	CurentWorld = Worlds[1];
	EditorCamera->ActiveInput(false);
	((QFAEditorWindow*)&GameViewport->HoldedWindowGame)->Viewports[0]->Root.removeAllUnit();

	for (size_t i = 0; i < ((QEditorWorld*)Worlds[0])->Actors.Length(); i++)
	{	
		QActor* actor = ((QEditorWorld*)Worlds[0])->Actors[i];
		QObject* object = QFAGameCode::GetAPI()->CreateObject(
			actor->GetClass()->GetId());

		if (object)
		{
			if (object->GetClass()->GetId() == QFAClass::ObjectClasses::StaticMeshActor)
				if (QFAMeshData* md = ((AEditorStaticMeshActor*)actor)->Mesh.GetMeshData())
					((AStaticMeshActor*)object)->SetMesh(md);
			
			Worlds[1]->AddActor((QActor*)object);
			((QActor*)object)->SetActorPosition(actor->GetActorPosition());
			((QActor*)object)->SetActorRotation(actor->GetActorRotation());
			((QActor*)object)->SetActorScale(actor->GetActorScale());
		}
	}	
	
	if (!((QFAEditorWindow*)&GameViewport->HoldedWindowGame)->Viewports[0]->CurentCamera)
	{
		ACameraEditor* ce = NewObject<ACameraEditor>();
		Worlds[1]->AddActor(ce);
		ce->SetWindowForInput(Window);
		ce->SetActorPosition(EditorCamera->GetActorPosition());
		ce->SetActorRotation(EditorCamera->GetActorRotation());
		ce->SetFov(EditorCamera->GetFov());
		ce->SetViewDistance(EditorCamera->GetViewDistance());
		ce->ActivateCamera(((QFAEditorWindow*)&GameViewport->HoldedWindowGame)->Viewports[0]);
	}	
}

void QFAEditorMainWindow::EndGame()
{		
	CurentWorld = Worlds[0];
	EditorCamera->ActiveInput(true);
	RunButton->SetText(PlaySymbol);
	((QEditorWorld*)Worlds[1])->DestroyWorld(true);
	((QFAEditorWindow*)&GameViewport->HoldedWindowGame)->Viewports[0]->CurentCamera = nullptr;
	((QFAEditorWindow*)&GameViewport->HoldedWindowGame)->Viewports[0]->Root.removeAllUnit();

	QEditorCameraComponent::ZerosMainCamera();
	Worlds[1] = NewObject<QWorld>();
	GameViewport->ChangeWindow();
	EditorCamera->ActivateCamera(((QFAEditorWindow*)&GameViewport->HoldedWindow)->Viewports[0]);
}

void QFAEditorMainWindow::ChangeTitle(std::u32string levelPAth)
{
	std::u32string windowTitle = std::u32string(U"GFAEngine ") +
		std::filesystem::path(levelPAth).replace_extension().filename().u32string();

	std::string utf8;
	icu::UnicodeString ucs = icu::UnicodeString::fromUTF32((UChar32*)windowTitle.c_str(), -1); // return UTF-16
	Window->SetWindowTitle(ucs.toUTF8String(utf8).c_str());
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
				FVector2D pos = GameViewport->GetPositionRelativeRegularWindow();
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
			QActor* actor = GameViewportInfo->PressedDelete();
			if (actor->IsValid())
				actor->Destroy();
		});

	Input->AddKeyRelease(EKey::S, "s_release", [this](EKey::Key key)
		{
			if (LeftCTRLPress && !OptionWindow)
			{
				if (Level)
				{
					Level->SaveLevel(Worlds[0]);
					return;
				}
				
				OptionWindow = new QFAEditorOptionWindow([this](std::u32string fileName)
				{ // QFAEditorOptionWindow delete self not need delete here
					Level = new QFAEditorLevel(fileName);
					Level->SaveLevel(Worlds[0]);	
					FileExplorer->UpdateFolderItemList();
					OptionWindow = nullptr;					
					ChangeTitle(fileName);
				},
				[this]()
				{
					OptionWindow = nullptr;
				});
			}
		});
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
					QActor* newActor = (QActor*)NewObject(newObjectClass->GetId());
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

void QFAEditorMainWindow::LoadLevel(QFAEditorFileStorage::QFAContentFile& cf)
{
	Worlds[0]->Destroy();
	Worlds[0] = ((QFAEditorLevel*)cf.file)->GetWorld(true);
	((QEditorWorld*)Worlds[0])->SetEditorActor(EditorCamera);
	CurentWorld = Worlds[0];
	ChangeTitle(cf.path);
	GameViewportInfo->RemoveAll();

	QEditorWorld* world = ((QEditorWorld*)Worlds[0]);
	for (size_t i = 0; i < world->Actors.Length(); i++)
	{ // rework in future
		GameViewportInfo->AddActor(world->Actors[i], QFAString::CharsTo32Chars(world->Actors[i]->GetClass()->GetName()), world->Actors[i]->GetClass()->GetId(), false);
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
	LoadCanvas = NewUI<QFAUICanvas>();
	TextList = NewUI<QFAUIList>();
	TextList->SetListType(QFAUIList::LTHorizonInner);
	Text = NewUI<QFAText>();
	LoadText = NewUI<QFAText>();
	LoadText_2 = NewUI<QFAText>();

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