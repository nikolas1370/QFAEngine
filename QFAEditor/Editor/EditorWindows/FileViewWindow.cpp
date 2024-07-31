#include "epch.h"
#include "FileViewWindow.h"
#include <EngineStuff/Window/EngineWindow.h>
#include <Object/World/World.h>
#include <UI/UIImage.h>
#include <UI/Canvas.h>	
#include <UI/UIList.h>	
#include "UI/Scroll.h"	 
#include <EditorUI/FileViewUnit.h>
#include <Tools/File/FileSystem.h>
#include <EngineStuff/Image.h>
#include <EngineStuff/Window/EngineViewport.h>
#include <filesystem>
#include <EngineClassesInterface.h>

QFAEditorFileViewWindow::QFAEditorFileViewWindow()
{
	
	Window = QFAEditorWindow::CreateEngineWindow(600, 600, "File View", true, true, [this]()
		{
			Closed = true;
		});
	DisplayImage = new QFAUIImage(nullptr, false);
	Canvas = new QFAUICanvas;
	TopListCanvas = new QFAUICanvas;
	TopListScroll = new QFAUIScroll;
	TopListScroll->SetScrollType(QFAUIScroll::STHorizon);
	TopListCanvas->AddUnit(TopListScroll);
	TopList = new QFAUIList;	
	TopListScroll->SetUnit(TopList);

	TopList->SetListType(QFAUIList::EListType::LTHorizon);
	TopList->SetUnitWidth(100);
	
	TopListScroll->SetWidth("100%");
	TopListScroll->SetHeight("25");
	
	TopListScroll->SetOverflow(QFAUIParent::Hidden);

	Canvas->AddUnit(DisplayImage);
	DisplayImage->SetWidth("100%");
	DisplayImage->SetHeight("95%");
	DisplayImage->SetTop("25");

	QFAEngineViewport* mainViewport = Window->GetViewport(0);
	
	mainViewport->AddUnit(TopListCanvas);
	mainViewport->AddUnit(Canvas);
	
	
	World.AddActor(&Camera);
	World.AddActor(&Actor);
	Camera.ActivateCamera(Window->GetViewport(0));
	Camera.SetWindowForInput(Window);
	Actor.SetRootComponent(&Mesh);

	TopList->Events.SetLeftMouseDown(&QFAEditorFileViewWindow::LeftMouse, this);
	TopList->Events.SetRightMouseDown(&QFAEditorFileViewWindow::RightMouse, this);
}

QFAEditorFileViewWindow::~QFAEditorFileViewWindow()
{

	/*
	* 	QFAUICanvas* Canvas;
	QFAUICanvas* TopListCanvas;

	QFAUIList* TopList;
	QFAUIScroll* TopListScroll;
	* 
		delete all Files
	
	*/
}

bool QFAEditorFileViewWindow::IsClosed()
{
	if (Closed)
		return true;
	
	Closed = ((QFAEditorWindow*)Window)->ShouldClose();
	return Closed; 
}

void QFAEditorFileViewWindow::AddFile(size_t fileId)
{
	if (CurentViewUnit)
		CurentViewUnit->Enable(false);

	Window->GetFocus();
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{
		if (Files[i].FileId == fileId)
		{
			CurentViewUnit = Files[i].viewUnit;
			CurentViewUnit->Enable(true);
			InFocus(&Files[i]);
			return;
		}
	}

	SFile sfile;
	sfile.FileId = QFAEditorFileStorage::GetFile(fileId).Id;
	if (!sfile.FileId)
		return;

	if (Files.size() == CurentEnableFolderCount)
	{
		sfile.viewUnit = new QFAEditorUIFileViewUnit;
		sfile.viewUnit->SetZIndex(10);
		Files.push_back(sfile);
	}
	else
		Files[CurentEnableFolderCount] = sfile;

	CurentViewUnit = Files[CurentEnableFolderCount].viewUnit;
	CurentEnableFolderCount++;
	UpdateList();
	InFocus(&Files[CurentEnableFolderCount - 1]);
	CurentViewUnit->Enable(true);
}


void QFAEditorFileViewWindow::InFocus(SFile* file)
{
	QFAContentManager::QFAContentFile& cf = QFAEditorFileStorage::GetFile(file->FileId);
	if (cf.fileType == QFAContentManager::QFAFileTypes::EFTMesh)
	{
		if (CurentActiveType == ECurentType::CTImage)
			DisplayImage->SetImage(nullptr);

		CurentActiveType = ECurentType::CTMesh;
		Camera.SetActorPosition(FVector(-1000, 0, 0));
		Camera.SetActorRotation(0);
		Actor.SetActorPosition(0);

		Mesh.SetMesh((QFAMeshData*)cf.file);
	}
	else
	{
		if (CurentActiveType == ECurentType::CTMesh)
			Mesh.SetMesh(nullptr);

		CurentActiveType = ECurentType::CTImage;
		for (size_t i = 0; i < CurentEnableFolderCount; i++)
		{
			if (Files[i].viewUnit == CurentViewUnit)
			{				
				DisplayImage->SetImage((QFAImage*)QFAEditorFileStorage::GetFile(Files[i].FileId).file);
				return;
			}
		}
	}
}

void QFAEditorFileViewWindow::UpdateList()
{
	TopList->removeAllUnit();
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{		
		Files[i].viewUnit->SetText(std::filesystem::path(QFAEditorFileStorage::GetFile(Files[i].FileId).path).filename().u32string());
		TopList->AddUnit(Files[i].viewUnit);
	}
}

void QFAEditorFileViewWindow::LeftMouse(QFAUIUnit* unit, void* _this)
{
	QFAEditorFileViewWindow* efvw = (QFAEditorFileViewWindow*)_this;
	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent->IsValid())
			return;

		if (parent->GetEditoUnitType() == QFAEditorUIType::FileViewUnit)
		{
			if (efvw->CurentViewUnit == parent)
				return;

			if (efvw->CurentViewUnit)
				efvw->CurentViewUnit->Enable(false);

			efvw->CurentViewUnit = (QFAEditorUIFileViewUnit*)parent;
			efvw->CurentViewUnit->Enable(true);

			for (size_t i = 0; i < efvw->CurentEnableFolderCount; i++)
			{
				if (efvw->Files[i].viewUnit == parent)
				{
					efvw->InFocus(&efvw->Files[i]);
					return;
				}
			}

			return;
		}

		parent = parent->GetParent();
	}
}

void QFAEditorFileViewWindow::RightMouse(QFAUIUnit* unit, void* _this)
{
	QFAEditorFileViewWindow* efvw = (QFAEditorFileViewWindow*)_this;
	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent->IsValid())
			return;

		if (parent->GetEditoUnitType() == QFAEditorUIType::FileViewUnit)
		{
			for (size_t i = 0; i < efvw->CurentEnableFolderCount; i++)
			{
				if (efvw->Files[i].viewUnit == parent)
				{
					efvw->Files.erase(efvw->Files.begin() + i);
					efvw->CurentEnableFolderCount--;
					if (efvw->CurentEnableFolderCount == 0)
						efvw->Window->Close();
					else
					{
						if (efvw->CurentViewUnit == parent)
						{
							efvw->CurentViewUnit = efvw->Files[0].viewUnit;
							efvw->InFocus(&efvw->Files[0]);
							efvw->CurentViewUnit->Enable(true);
						}
						
						efvw->UpdateList();
					}

					return;
				}
			}



			return;
		}

		parent = parent->GetParent();
	}
}