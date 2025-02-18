#include "epch.h"
#include "FileViewWindow.h"
#include <Core/EngineStuff/Window/EngineWindow.h>
#include <Object/World/World.h>
#include <UI/UIImage.h>
#include <UI/Canvas.h>	
#include <UI/UIList.h>	
#include "UI/Scroll.h"	 
#include <EditorUI/FileViewUnit.h>
#include <Tools/File/FileSystem.h>
#include <Core/EngineStuff/Image.h>
#include <Core/EngineStuff/Window/EngineViewport.h>
#include <filesystem>
#include <EngineClassesInterface.h>

QFAEditorFileViewWindow::QFAEditorFileViewWindow()
{
	
	Window = QFAEditorWindow::CreateEngineWindow(600, 600, "File View", true, true, [this]()
		{
			Closed = true;
		});
	DisplayImage = NewUI<QFAUIImage>(nullptr, false);
	Canvas = NewUI <QFAUICanvas>();
	TopListCanvas = NewUI<QFAUICanvas>();
	TopListScroll = NewUI<QFAUIScroll>();
	TopListScroll->SetScrollType(QFAUIScroll::STHorizon);
	TopListCanvas->AddUnit(TopListScroll);
	TopList = NewUI<QFAUIList>();
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
	
	SetLeftMouse();
	SetRightMouse();
}

QFAEditorFileViewWindow::~QFAEditorFileViewWindow()
{
	DisplayImage->Destroy();
	Canvas->Destroy();
	TopListCanvas->Destroy();
	TopListScroll->Destroy();
	TopList->Destroy();
	/*
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
		sfile.viewUnit = NewUI<QFAEditorUIFileViewUnit>();
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

void QFAEditorFileViewWindow::SetLeftMouse()
{
	TopList->Events.SetLeftMouseDown([this](QFAUIUnit* unit)
	{
		QFAUIUnit* parent = unit;
		while (true)
		{
			if (!parent->IsValid())
				return;

			if (parent->GetEditoUnitType() == QFAEditorUIType::FileViewUnit)
			{
				if (CurentViewUnit == parent)
					return;

				if (CurentViewUnit)
					CurentViewUnit->Enable(false);

				CurentViewUnit = (QFAEditorUIFileViewUnit*)parent;
				CurentViewUnit->Enable(true);

				for (size_t i = 0; i < CurentEnableFolderCount; i++)
				{
					if (Files[i].viewUnit == parent)
					{
						InFocus(&Files[i]);
						return;
					}
				}

				return;
			}

			parent = parent->GetParent();
		}
	});	
}

void QFAEditorFileViewWindow::SetRightMouse()
{
	TopList->Events.SetRightMouseDown([this](QFAUIUnit* unit)
	{
		QFAUIUnit* parent = unit;
		while (true)
		{
			if (!parent->IsValid())
				return;

			if (parent->GetEditoUnitType() == QFAEditorUIType::FileViewUnit)
			{
				for (size_t i = 0; i < CurentEnableFolderCount; i++)
				{
					if (Files[i].viewUnit == parent)
					{
						Files.erase(Files.begin() + i);
						CurentEnableFolderCount--;
						if (CurentEnableFolderCount == 0)
							Window->Close();
						else
						{
							if (CurentViewUnit == parent)
							{
								CurentViewUnit = Files[0].viewUnit;
								InFocus(&Files[0]);
								CurentViewUnit->Enable(true);
							}

							UpdateList();
						}

						return;
					}
				}

				return;
			}

			parent = parent->GetParent();
		}
	});	
}