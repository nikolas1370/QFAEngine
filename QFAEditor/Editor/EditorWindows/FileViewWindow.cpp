#include "epch.h"
#include "FileViewWindow.h"
#include <Render/Window/Window.h>
#include <Object/World/World.h>
#include <Render/UI/UIImage.h>
#include <Render/UI/Canvas.h>	
#include <Render/UI/UIList.h>	
#include "Render/UI/Scroll.h"	 
#include <EditorUI/FileViewUnit.h>
#include <Tools/File/FileSystem.h>
#include <EditorFileTypes.h>
#include <Render/Image.h>
#include <Render/Window/Viewport.h>
#include <filesystem>


QFAEditorFileViewWindow::QFAEditorFileViewWindow()
{
	Window = new QFAWindow(600, 600, "File View", true , true, [this]()
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

	TopList->SetPosition(0, 0);
	TopList->SetListType(QFAUIList::EListType::LTHorizon);
	TopList->SetUnitWidth(100);

	

	QFAUISlot::SCanvasSlot Slot;
	Slot.Height = 25;
	Slot.HeightInPixel = true;
	Slot.Width = 1.0f;
	TopListScroll->SetSlot(&Slot);
	
	TopListScroll->SetOverflow(QFAUIParent::Hidden);

	Canvas->AddUnit(DisplayImage);
	Slot.y = 25;
	Slot.yInPixel = true;
	Slot.HeightInPixel = false;
	Slot.Width = 1.0f;
	Slot.Height = 0.95f;
	DisplayImage->SetSlot(&Slot);

	QFAViewport* mainViewport = Window->GetViewport(0);
	
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

	Closed = Window->ShouldClose();
	return Closed; 
}

void QFAEditorFileViewWindow::AddFile(size_t fileId)
{
	if (CurentViewUnit)
		CurentViewUnit->Enable(false);

	Window->GetFocus();
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{
		if (Files[i].ef.id == fileId)
		{
			CurentViewUnit = Files[i].viewUnit;
			CurentViewUnit->Enable(true);
			InFocus(&Files[i]);
			return;
		}
	}

	SFile sfile;
	sfile.ef = QFAEditorFileStorage::GetFile(fileId);
	if (!sfile.ef.id)
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
	if (file->ef.fileType == QFAEditorFileTypes::EFTMesh)
	{
		if (CurentActiveType == ECurentType::CTImage)
			DisplayImage->SetImage(nullptr);

		CurentActiveType = ECurentType::CTMesh;
		Camera.SetActorPosition(FVector(-1000, 0, 0));
		Camera.SetActorRotation(0);
		Actor.SetActorPosition(0);

		Mesh.SetMesh((MeshData*)file->ef.file);
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
				DisplayImage->SetImage((QFAImage*)Files[i].ef.file);
				return;
			}
		}
	}
}

void QFAEditorFileViewWindow::UpdateList()
{
	TopList->removeAllUnit();
	QFAUISlot::SListSlot listSlot;
	listSlot.marginLeft = 10;
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{
		Files[i].viewUnit->SetText(std::filesystem::path(Files[i].ef.path).filename().u32string());
		TopList->AddUnit(Files[i].viewUnit);
		Files[i].viewUnit->SetSlot(&listSlot);
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