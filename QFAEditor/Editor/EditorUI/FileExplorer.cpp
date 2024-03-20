#include "FileExplorer.h"
#include <Tools/VulkanSuff.h>
#include <Render/UI/UIList.h>
#include <Render/UI/Canvas.h>
#include <Render/UI/Text.h>
#include <Render/UI/Grid.h>
#include <Render/UI/Scroll.h>
#include <Tools/File/FileSystem.h>
#include "ExplorerFolderUnit.h"
#include <EditorWindows/MainEditorWindow.h>
#include <Render/Time.h>

#include <filesystem>
#include <stb_image.h>

#include <unicode/unistr.h>
#include <EditorFileTypes.h>
#include <Tools/File/ModelLoader.h>
#include <EditorWindows/FileViewWindow.h>
#include <EditorFileStorage.h>

QFAUIEditorFileExplorer::QFAUIEditorFileExplorer(QFAWindow *window)
{
	Window = window;
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::FileExplorer;
	SetBackgroundColor(FileExplorerBackgroundColor);

	Events.SetForwardMouseDown(&QFAUIEditorFileExplorer::NextFolderButton, this);
	Events.SetBackwardMouseDown(&QFAUIEditorFileExplorer::PreviousFolderButton, this);

	CreateTop();
	CreateBottom();
	AddHiddenChild(FileExplorerTop);
	AddHiddenChild(FileExplorerBottom);	
	QFAFileSystem::FolderUnit fu;
	fu.id = 1;// 1 is defauld id for flder Content
	fu.IsFolder = true;
	fu.path = MainFolder;
	CurentFolder = fu;
	PathHistory.push_back(fu);
	PathChanged();
}

QFAUIEditorFileExplorer::~QFAUIEditorFileExplorer()
{
}

void QFAUIEditorFileExplorer::CreateTop()
{
	FileExplorerTop = new QFAUIList;
	FileExplorerTop->SetStretchLastUnit(true);
	FileExplorerTop->SetListType(QFAUIList::LTHorizon);
	FileExplorerTop->SetUnitWidth(25); 

	BackButton = new QFAText;
	ForwardButton = new QFAText;
	BackButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	ForwardButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	ForwardButton->SetText(std::u32string(&ForwardButtonIconCode, 1));
	BackButton->SetText(std::u32string(&BackButtonIconCode, 1));
	BackButton->SetTextSize(20);
	ForwardButton->SetTextSize(20);
	BackButton->SetTextAlign(QFAText::TACenter);
	ForwardButton->SetTextAlign(QFAText::TACenter);
	BackButton->SetTextColor((unsigned char)106, (unsigned char)106, (unsigned char)106);
	ForwardButton->SetTextColor((unsigned char)106, (unsigned char)106, (unsigned char)106);

	ForwardButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::NextFolderButton, this);
	BackButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::PreviousFolderButton, this);

	QFAUISlot::SListSlot ListSlot;
	ListSlot.marginLeft = 10;
	PathTextScroll = new QFAUIScroll;
	PathTextScroll->SetScrollType(QFAUIScroll::STHorizon);
	PathTextScroll->SetSlot(&ListSlot);
	

	PathText = new QFAText;
	PathTextScroll->SetUnit(PathText);
	PathText->SetTextSize(18);	
	PathText->SetOverflowWrap(QFAText::OWNone);
	

	FileExplorerTop->AddUnit(BackButton);
	FileExplorerTop->AddUnit(ForwardButton);
	FileExplorerTop->AddUnit(PathTextScroll);
}

void QFAUIEditorFileExplorer::CreateBottom()
{
	FileExplorerBottom = new QFAUICanvas;

	FolderItemListScroll = new QFAUIScroll;
	FolderItemListScroll->SetScrollType(QFAUIScroll::STVertical);

	FolderItemList = new QFAUIGrid;
	FolderItemListScroll->SetUnit(FolderItemList);
	FolderItemList->SetPositionType(QFAUIGrid::UPTAuto);
	FolderItemList->SetMin(100);

	FolderItemList->SetOffsets(10, 10);
	FolderItemList->SetRation(1.5);

	QFAUISlot::SCanvasSlot slot;

	slot.Height = 1.0f;
	slot.Width = 1.0f;
	slot.x = 0.0f;
	slot.y = 0.0f;
	FolderItemListScroll->SetSlot(&slot);
	FileExplorerBottom->AddUnit(FolderItemListScroll);


	FolderItemList->Events.SetInFocus(&QFAUIEditorFileExplorer::FolderItemListInFocus, this);
	FolderItemList->Events.SetOutFocus(&QFAUIEditorFileExplorer::FolderItemListOutFocus, this);
	FolderItemList->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::FolderItemListLeftMouseDown, this);
}

void QFAUIEditorFileExplorer::UpdateFolderItemList()
{	
	
	


	folderContents.clear();
	FolderItemList->removeAllUnit();
	QFAEditorFileStorage::GetFolderContents(CurentFolder.id, folderContents);
	folderUnitInUse = 0;
	for (size_t i = 0; i < folderContents.size(); i++)
	{
		if (folderUnitInUse == FolderUnitList.size())
		{
			FolderUnitList.push_back(new QFAEditorExplorerFolderUnit);
		}

		FolderItemList->AddUnit(FolderUnitList[folderUnitInUse]);
		FolderUnitList[folderUnitInUse]->ChangeImage(folderContents[i].IsFolder);
		FolderUnitList[folderUnitInUse]->ChangeText(folderContents[i].name);
		folderUnitInUse++;		
	}	
}

void QFAUIEditorFileExplorer::FolderItemListLeftMouseDown(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)(_this);

	if (unit == thisUnit->FolderItemListSelectUnit)
		return;

	if (thisUnit == unit)
	{
		thisUnit->FolderItemListSelectUnit = nullptr;
		return;
	}

	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent || parent == (QFAUIUnit*)_this)
			break;
		
		if (parent->GetEditoUnitType() == QFAEditorUIType::ExplorerFolderUnit)
		{
			if (parent == thisUnit->FolderItemListSelectUnit &&
				QTime::GetSystemTime() - thisUnit->LastLeftMouseDownTime < thisUnit->MouseDownMaxTime)
			{				
				for (size_t i = 0; i < thisUnit->FolderUnitList.size(); i++)
				{
					if (thisUnit->FolderUnitList[i] == parent)
					{					
						if (thisUnit->folderContents[i].IsFolder)
						{							
							thisUnit->LastLeftMouseDownTime = 0;
							thisUnit->FolderItemListSelectUnit->SetBackgroundColor(thisUnit->InFocusUnitColor);
							thisUnit->FolderItemListSelectUnit = nullptr;
							thisUnit->NextFolder(thisUnit->folderContents[i]);
						}
						else
						{
							QFAUIEditorFileExplorer* fe = (QFAUIEditorFileExplorer*)_this;
							if (fe->FileViewWindow)
							{
								if (fe->FileViewWindow->IsClosed())
								{
									delete fe->FileViewWindow;
									fe->FileViewWindow = new QFAEditorFileViewWindow();
								}

								fe->FileViewWindow->AddFile(thisUnit->folderContents[i].id);
							}
							else
							{
								fe->FileViewWindow = new QFAEditorFileViewWindow();
								fe->FileViewWindow->AddFile(thisUnit->folderContents[i].id);
							}
						}

						return;
					}
				}
			}
			else
			{
				if (thisUnit->FolderItemListSelectUnit)
					thisUnit->FolderItemListSelectUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));

				thisUnit->LastLeftMouseDownTime = QTime::GetSystemTime();
				thisUnit->FolderItemListSelectUnit = (QFAEditorExplorerFolderUnit*)parent;
				thisUnit->FolderItemListSelectUnit->SetBackgroundColor(thisUnit->SelectUnit);
			}
			
			break;
		}

		parent = parent->GetParent();
	}
}

void QFAUIEditorFileExplorer::FolderItemListInFocus(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)(_this);
	if (thisUnit->FolderItemListInFocusUnit && thisUnit->FolderItemListSelectUnit != thisUnit->FolderItemListInFocusUnit)
		thisUnit->FolderItemListInFocusUnit->SetBackgroundColor(QFAColorF(0.0f, 0.0f, 0.0f, 0.0f));

	thisUnit->FolderItemListInFocusUnit = nullptr;
	if (_this == unit || unit == thisUnit->FolderItemListSelectUnit)
		return;

	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent || parent == (QFAUIUnit*)_this )
			break;
		
		if (parent->GetEditoUnitType() == QFAEditorUIType::ExplorerFolderUnit)
		{
			if (thisUnit->FolderItemListSelectUnit == parent)
				return;

			thisUnit->FolderItemListInFocusUnit = (QFAEditorExplorerFolderUnit*)parent;
			thisUnit->FolderItemListInFocusUnit->SetBackgroundColor(thisUnit->InFocusUnitColor);
			break;
		}

		parent = parent->GetParent();
	}
}

void QFAUIEditorFileExplorer::FolderItemListOutFocus(void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)(_this);
	if (thisUnit->FolderItemListInFocusUnit && thisUnit->FolderItemListSelectUnit != thisUnit->FolderItemListInFocusUnit)
		thisUnit->FolderItemListInFocusUnit->SetBackgroundColor(QFAColorF(0.0f, 0.0f, 0.0f, 0.0f));

	thisUnit->FolderItemListInFocusUnit = nullptr;
}

void QFAUIEditorFileExplorer::NextFolder(QFAFileSystem::FolderUnit nextFolder)
{
	if (!QFAFileSystem::exists(nextFolder.path))
		return;
	
	

	if (PathHistory.size() > CurentPathHistoryIndex + 1 &&
		PathHistory[CurentPathHistoryIndex + 1].id == nextFolder.id)
	{
		CurentPathHistoryIndex++;
		CurentFolder = nextFolder;
		BackButton->SetTextColor(ButoonOnColor);
		if(PathHistory.size() > CurentPathHistoryIndex + 1)
			ForwardButton->SetTextColor(ButoonOnColor);
		else
			ForwardButton->SetTextColor(ButoonOffColor);
	}
	else
	{
		CurentPathHistoryIndex++;
		PathHistory.resize(CurentPathHistoryIndex);
		PathHistory.push_back(nextFolder);
		CurentFolder = nextFolder;
		ForwardButton->SetTextColor(ButoonOffColor);
		BackButton->SetTextColor(ButoonOnColor);
	}
	
	PathChanged();
}

void QFAUIEditorFileExplorer::NextFolderButton(QFAUIUnit* unit, void* _this)
{	
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;

	if (thisUnit->CurentPathHistoryIndex + 1 >= thisUnit->PathHistory.size())
		return;

	thisUnit->CurentPathHistoryIndex++;
	thisUnit->CurentFolder = thisUnit->PathHistory[thisUnit->CurentPathHistoryIndex];
	thisUnit->BackButton->SetTextColor(thisUnit->ButoonOnColor);
	if (thisUnit->CurentPathHistoryIndex + 1 >= thisUnit->PathHistory.size())
		thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOffColor);
	else
		thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOnColor);

	thisUnit->PathChanged();
}

void QFAUIEditorFileExplorer::PreviousFolderButton(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;	
	if (thisUnit->CurentPathHistoryIndex == 0)
		return;

	thisUnit->CurentPathHistoryIndex--;
	thisUnit->CurentFolder = thisUnit->PathHistory[thisUnit->CurentPathHistoryIndex];
	thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOnColor);
	if(thisUnit->CurentPathHistoryIndex == 0)
		thisUnit->BackButton->SetTextColor(thisUnit->ButoonOffColor);
	else
		thisUnit->BackButton->SetTextColor(thisUnit->ButoonOnColor);

	thisUnit->PathChanged();
}

void QFAUIEditorFileExplorer::DropFiles(int path_count, const char* paths[])
{
	/*
	
	
	
	
	
	rewrite
	
	
	
	
	*/


	std::filesystem::path curentPath(CurentFolder.path);
	curentPath.append("");// add "\\" in path
	for (size_t i = 0; i < path_count; i++)
	{
		icu::UnicodeString ucs = icu::UnicodeString::fromUTF8(paths[i]);
		UErrorCode kkl = U_ZERO_ERROR;
		DropPath.resize(ucs.length());

		int32_t strLen = ucs.toUTF32(DropPath.data(), DropPath.size(), kkl);
		std::u32string str;
		str.resize(strLen);
		for (size_t j = 0; j < ucs.length(); j++)
			str[j] = static_cast<char32_t>(DropPath[j]);

		QFAFile file;
		//QFAFileSystem::LoadFile(str, &file);
		std::filesystem::path path(str);
		std::filesystem::path ext = path.extension();
		if (std::filesystem::is_directory(path))
		{
			std::cout << "DropFile not file\n";
			continue;
		}

		QFAEditorFile ef;
		ef.version = EditorFileVersion;
		if (ext == ".obj" || ext == ".fbx")
		{
			/*
				QFAEditorFile
				MeshData::SMeshInfo
				MeshData::FramesData
			*/

			/*
				create fun when load model and get aiScene check if valid
				and after use aiScene like SeparateModel or solid
			*/
			MeshData* md = QFAModelLoader::LoadModel(paths[i]);
			if (!md)
			{
				std::cout << "DropFile not load\n";
				continue;
			}

			ef.type = QFAEditorFileTypes::EFTMesh;
			path.replace_extension(".qfa");
			curentPath.replace_filename(path.filename());

			QFAFileSystem::WriteFile(curentPath.u32string(), &ef, sizeof(ef));
			
			MeshData::SMeshInfo mi = md->GetMeshInfo();
			QFAFile writeFile;
			QFAFileSystem::OpenFile(curentPath.u32string(), &writeFile, false);
			QFAFileSystem::AppendFile(&writeFile, &mi, sizeof(mi));

			QFAFileSystem::AppendFile(&writeFile, md->GetFrameData(), mi.AmountData);
			QFAFileSystem::CloseFile(&writeFile);
		}
		else if (ext == ".jpg" || ext == ".png")
		{
			/*
				QFAEditorFile
				raw Image			
			*/
			int texWidth, texHeight, texChannels;
			QFAFile rawImageFile;
			if (QFAFileSystem::LoadFile(str, &rawImageFile))
			{
				std::cout << "DropFile not load\n";
				continue;
			}

			stbi_uc* pixels = stbi_load_from_memory((stbi_uc*)rawImageFile.GetData(), rawImageFile.GetFileSize(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);				
			if (!pixels) // check if rawImageFile is image
			{
				std::cout << "DropFile not load\n";
				continue;
			}
			
			stbi_image_free(pixels);
			path.replace_extension(".qfa");
			curentPath.replace_filename(path.filename());

			ef.type = QFAEditorFileTypes::EFTImage;
			QFAFileSystem::WriteFile(curentPath.u32string(), &ef, sizeof(ef));

			QFAFile writeFile;			
			QFAFileSystem::OpenFile(curentPath.u32string(), &writeFile, false);
			QFAFileSystem::AppendFile(&writeFile, rawImageFile.GetData(), rawImageFile.GetFileSize());
			QFAFileSystem::CloseFile(&writeFile);			
		}
		else
			std::cout << "DropFile not support extension\n";

	}

	UpdateFolderItemList();
}

void QFAUIEditorFileExplorer::PathChanged()
{
	PathText->SetText(CurentFolder.path);
	UpdateFolderItemList();
}

void QFAUIEditorFileExplorer::MySlotChange(QFAUIUnit* unit)
{

}

void QFAUIEditorFileExplorer::ChangeSize(unsigned int w, unsigned int h)
{	
	SetChildSize(FileExplorerTop, w, h);
	SetChildSize(FileExplorerBottom, w, h >= FileExplorerTopHeight ? h - FileExplorerTopHeight : 0);
}

void QFAUIEditorFileExplorer::ChangePosition(int x, int y)
{
	SetChildPosition(FileExplorerTop, x, y );
	SetChildPosition(FileExplorerBottom, x, y + FileExplorerTopHeight);
}

float QFAUIEditorFileExplorer::UpdateInnerHeight()
{
	return Height;
}

float QFAUIEditorFileExplorer::UpdateInnerWidth()
{
	return Width;
}