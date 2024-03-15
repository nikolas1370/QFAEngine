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
#include <stb_image.h>
#include <filesystem>


QFAEditorFileViewWindow::QFAEditorFileViewWindow()
{
	Window = new QFAWindow(600, 600, "File View", [this]()
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
		delete all Folders
	
	*/
}

bool QFAEditorFileViewWindow::IsClosed()
{
	if (Closed)
		return true;

	Closed = Window->ShouldClose();
	return Closed; 
}

void QFAEditorFileViewWindow::AddFile(std::u32string qfaFilePAth)
{
	if (CurentViewUnit)
		CurentViewUnit->Enable(false);

	Window->GetFocus();
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{
		if (Folders[i].path == qfaFilePAth)
		{		
			CurentViewUnit = Folders[i].viewUnit;
			CurentViewUnit->Enable(true);			
			InFocus(&Folders[i]);
			return;
		}
	}
	
	SFile sfile;
	if (!LoadFile(qfaFilePAth, sfile))
		stopExecute("not load");

	if (Folders.size() == CurentEnableFolderCount)
	{
		sfile.viewUnit = new QFAEditorUIFileViewUnit;
		sfile.viewUnit->SetZIndex(10);
		Folders.push_back(sfile);
	}
	else
		Folders[CurentEnableFolderCount] = sfile;

	CurentViewUnit = Folders[CurentEnableFolderCount].viewUnit;
	CurentEnableFolderCount++;
	UpdateList();
	InFocus(&Folders[CurentEnableFolderCount - 1]);
	CurentViewUnit->Enable(true);
}

void QFAEditorFileViewWindow::InFocus(SFile* file)
{
	if (file->mesh)
	{
		if (CurentActiveType == ECurentType::CTImage)
			DisplayImage->SetImage(nullptr);

		CurentActiveType = ECurentType::CTMesh;
		Camera.SetActorPosition(FVector(-1000, 0, 0));
		Camera.SetActorRotation(0);
		Actor.SetActorPosition(0);
		Actor.SetRootComponent(file->mesh);
	}
	else
	{
		if (CurentActiveType == ECurentType::CTMesh)
			Actor.SetRootComponent(nullptr);

		CurentActiveType = ECurentType::CTImage;
		for (size_t i = 0; i < CurentEnableFolderCount; i++)
		{
			if (Folders[i].viewUnit == CurentViewUnit)
			{
				DisplayImage->SetImage(Folders[i].image);
				return;
			}
		}
	}
}

bool QFAEditorFileViewWindow::LoadFile(std::u32string qfaFilePAth, SFile& sfile)
{
	sfile.path = qfaFilePAth;
	QFAFile* file = new QFAFile;
	sfile.file = file;
	if (QFAFileSystem::LoadFile(qfaFilePAth, file))
		stopExecute("not load");

	QFAEditorFile* eFile = (QFAEditorFile*)file->GetData();
	if (eFile->type == QFAEditorFileTypes::EFTImage)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load_from_memory(((const unsigned char*)eFile + sizeof(QFAEditorFile)), (int)file->GetDataSize() - sizeof(QFAEditorFile), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels)
			stopExecute("");

		QFAImage::SImageCreateInfo ici;
		ici.Width = texWidth;
		ici.Height = texHeight;
		ici.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ici.createBuffer = false;
		QFAImage* image = new QFAImage(ici);

		image->SetImage(pixels);		
		stbi_image_free(pixels);

		sfile.image = image;
	}
	else if (eFile->type == QFAEditorFileTypes::EFTMesh)
	{// sizeof(QFAEditorFile)
		MeshData* meshData = new MeshData(
			(MeshData::SMeshInfo*)((char*)eFile + sizeof(QFAEditorFile)),
			((char*)eFile + sizeof(QFAEditorFile) + sizeof(MeshData::SMeshInfo))
		);
		
		QStaticMesh* mesh = new QStaticMesh(meshData);
		sfile.mesh = mesh;
	} 
	else
		stopExecute("not support");

	return true;
}

void QFAEditorFileViewWindow::UpdateList()
{
	TopList->removeAllUnit();
	QFAUISlot::SListSlot listSlot;
	listSlot.marginLeft = 10;
	for (size_t i = 0; i < CurentEnableFolderCount; i++)
	{
		Folders[i].viewUnit->SetText(std::filesystem::path(Folders[i].path).filename().u32string());
		TopList->AddUnit(Folders[i].viewUnit);
		Folders[i].viewUnit->SetSlot(&listSlot);
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
				if (efvw->Folders[i].viewUnit == parent)
				{
					efvw->InFocus(&efvw->Folders[i]);
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
		
			//efvw->CurentViewUnit;

			for (size_t i = 0; i < efvw->CurentEnableFolderCount; i++)
			{
				if (efvw->Folders[i].viewUnit == parent)
				{					
					SFile sfile = efvw->Folders[i];
					efvw->Folders.erase(efvw->Folders.begin() + i);
					efvw->Folders.push_back(sfile);
					efvw->CurentEnableFolderCount--;
					if (efvw->CurentEnableFolderCount == 0)
						efvw->Window->Close();
					else
					{
						/*
						
							удалить теперішній і подив я перейде на перед
						
						*/
						if (efvw->CurentViewUnit == parent)
						{
							efvw->CurentViewUnit = efvw->Folders[0].viewUnit;
							efvw->InFocus(&efvw->Folders[0]);
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