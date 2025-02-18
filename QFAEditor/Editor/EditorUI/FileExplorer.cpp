#include "epch.h"
#include "FileExplorer.h"
#include <Core/EngineStuff/VulkanSuff.h>
#include <UI/UIList.h>
#include <UI/Canvas.h>
#include <UI/Text.h>
#include <UI/Grid.h>
#include <UI/Scroll.h>
#include <Tools/File/FileSystem.h>
#include "ExplorerFolderUnit.h"
#include <EditorWindows/MainEditorWindow.h>
#include <Core/Time.h>
#include <filesystem>
#include <EditorWindows/FileViewWindow.h>
#include "../GameCodeTool/GameCodeCompiler.h"
#include <Tools/String.h>
#include <UI/SelectUnit.h>


QFAUIEditorFileExplorer::QFAUIEditorFileExplorer(QFAEngineWindow *window, std::function <void(bool isCppClass, size_t fileId)> dragFun, std::function <void(QFAEditorFileStorage::QFAContentFile& cf)> levelLoad)
{
	DragFun = dragFun;
	LevelLoad = levelLoad;
	Window = window;
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::FileExplorer;
	SetBackgroundColor(FileExplorerBackgroundColor);

	CreateTop();
	CreateMiddle();
	CreateBottom();		
	CreateCppTop();

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
	FileExplorerTop->Destroy();
	BackButton->Destroy();
	ForwardButton->Destroy();
	PathTextScroll->Destroy();
	PathText->Destroy();
	FileExplorerMiddle->Destroy();
	SelectGrid->Destroy();
	FileExplorerBottom->Destroy();
	ExplorerButton->Destroy();
	CppButton->Destroy();
	CppCanvas->Destroy();
}

void QFAUIEditorFileExplorer::CreateTop()
{
	FileExplorerTop = NewUI<QFAUIList>();
	FileExplorerTop->SetWidth("100%");
	FileExplorerTop->SetHeight("25");
	FileExplorerTop->SetListType(QFAUIList::LTHorizon);
	FileExplorerTop->SetUnitWidth(25); 

	BackButton = NewUI<QFAText>();
	ForwardButton = NewUI<QFAText>();
	BackButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	ForwardButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	ForwardButton->SetText(ForwardButtonIconCode);
	BackButton->SetText(BackButtonIconCode);
	BackButton->SetTextSize(20);
	ForwardButton->SetTextSize(20);
	BackButton->SetTextAlign(QFAText::TACenter);
	ForwardButton->SetTextAlign(QFAText::TACenter);
	BackButton->SetTextColor(ButoonOffColor);
	ForwardButton->SetTextColor(ButoonOffColor);
	

	PathTextScroll = NewUI<QFAUIScroll>();
	PathTextScroll->SetWidth("100% - 50"); // 50 == FileExplorerTop->SetUnitWidth for SetUnitWidth + ForwardButton
	PathTextScroll->SetHeight("100%");
	PathTextScroll->SetScrollType(QFAUIScroll::STHorizon);

	PathText = NewUI<QFAText>();
	PathTextScroll->SetUnit(PathText);
	PathText->SetTextSize(18);	
	PathText->SetOverflowWrap(QFAText::OWNone);
	
	FileExplorerTop->AddUnit(BackButton);
	FileExplorerTop->AddUnit(ForwardButton);
	FileExplorerTop->AddUnit(PathTextScroll);	
	AddHiddenChild(FileExplorerTop);

	SetNextFolderButton();	
	SetPreviousFolderButton();
}

void QFAUIEditorFileExplorer::CreateMiddle()
{
	FileExplorerMiddle = NewUI<QFAUICanvas>();
	FileExplorerMiddle->SetWidth("100%");
	FileExplorerMiddle->SetHeight("100% - 30 - 30");// (30 top) - (30 bottom)
	FileExplorerMiddle->SetTop("30");

	SelectGrid = NewUI<QFAUISelectGrid>();

	SelectGrid->SetScrollType(QFAUIScroll::STVertical);
	SelectGrid->SetPositionType(QFAUIGrid::UPTAuto);
	SelectGrid->SetMin(100);
	SelectGrid->SetOffsets(10, 10);
	SelectGrid->SetRation(1.5);
	SelectGrid->FocusColor = InFocusUnitColor;
	SelectGrid->SelectColor = SelectUnit;
	SelectGrid->SelectLostFocusColor = SelectUnitNotFocus;

	SelectGrid->SetWidth("100%");
	SelectGrid->SetHeight("100%");
	FileExplorerMiddle->AddUnit(SelectGrid);

	SelectGrid->SelectEvent.LeftMouseDown = ([this](QFAUIParent* unit, void* extra)
		{
			NotifyMainEditorWindowDrag((QFAEditorExplorerFolderUnit*)unit);
		});

	SelectGrid->SelectEvent.DobleClick = ([this](QFAUIParent* unit, void* extra)
		{
			for (size_t i = 0; i < FolderUnitList.size(); i++)
			{
				if (FolderUnitList[i] == unit)
				{
					if (folderContents[i].IsFolder)
					{
						unit->SetBackgroundColor(InFocusUnitColor);
						NextFolder(folderContents[i]);
					}
					else
					{
						QFAEditorFileStorage::QFAContentFile& cf = QFAEditorFileStorage::GetFile(folderContents[i].id);
						if (cf.Id < 0)
							stopExecute("");

						if (cf.fileType == QFAContentManager::QFAFileTypes::EFTLevel)
							return LevelLoad(cf);

						if (FileViewWindow)
						{
							if (FileViewWindow->IsClosed())
							{
								delete FileViewWindow;
								FileViewWindow = new QFAEditorFileViewWindow();
							}

							FileViewWindow->AddFile(folderContents[i].id);
						}
						else
						{
							FileViewWindow = new QFAEditorFileViewWindow();
							FileViewWindow->AddFile(folderContents[i].id);
						}
					}

					return;
				}
			}
		});
	
	AddHiddenChild(FileExplorerMiddle);
}

void QFAUIEditorFileExplorer::CreateBottom()
{
	FileExplorerBottom = NewUI<QFAUIList>();
	FileExplorerBottom->SetWidth("100%");
	FileExplorerBottom->SetHeight("30");
	FileExplorerBottom->SetTop("100% - 30");

	FileExplorerBottom->SetListType(QFAUIList::LTHorizon);
	FileExplorerBottom->SetUnitWidth(25);

	ExplorerButton = NewUI<QFAText>();
	CppButton = NewUI<QFAText>();
	CppButton->SetLeft("20");
	ExplorerButton->SetLeft("10");
	ExplorerButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	CppButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	CppButton->SetText(CppFileButtonIconCode);
	ExplorerButton->SetText(FolderButtonIconCode);
	ExplorerButton->SetTextSize(30);
	CppButton->SetTextSize(30);
	ExplorerButton->SetTextAlign(QFAText::TACenter);
	CppButton->SetTextAlign(QFAText::TACenter);
	ExplorerButton->SetTextColor(ButoonOnColor); // Explorer show first
	CppButton->SetTextColor(ButoonOffColor);

	SetCppButtonE();
	SetExplorerButtonE();


	FileExplorerBottom->AddUnit(ExplorerButton);
	FileExplorerBottom->AddUnit(CppButton);
	
	AddHiddenChild(FileExplorerBottom);
}

void QFAUIEditorFileExplorer::CreateCppTop()
{
	CppCanvas = NewUI<QFAUICanvas>();
	CppCanvas->SetWidth("100%");
	CppCanvas->SetHeight("100% - 30"); // 30 FileExplorerBottom.height
	CppItemList = NewUI<QFAUISelectGrid>();

	CppItemList->SetScrollType(QFAUIScroll::STVertical);
	CppItemList->SetPositionType(QFAUIGrid::UPTAuto);
	CppItemList->SetMin(100);
	CppItemList->SetOffsets(10, 10);
	CppItemList->SetRation(1.5);
	CppItemList->FocusColor = InFocusUnitColor;
	CppItemList->SelectColor = SelectUnit;
	CppItemList->SelectLostFocusColor = SelectUnitNotFocus;

	CppItemList->SetWidth("100%");
	CppItemList->SetHeight("100%");
	CppCanvas->AddUnit(CppItemList);

	CppCanvas->SetEnable(false);
	AddHiddenChild(CppCanvas);

	CppItemList->SelectEvent.LeftMouseDown = ([this](QFAUIParent* unit, void* extra)
		{
			NotifyMainEditorWindowDrag((QFAEditorExplorerFolderUnit*)unit);
		});
}

void QFAUIEditorFileExplorer::UpdateFolderItemList()
{	
	folderContents.clear();
	SelectGrid->RemoveAllUnit();
	QFAEditorFileStorage::GetFolderContents(CurentFolder.id, folderContents);
	folderUnitInUse = 0;
	for (size_t i = 0; i < folderContents.size(); i++)
	{
		if (folderUnitInUse == FolderUnitList.size())
			FolderUnitList.push_back(NewUI<QFAEditorExplorerFolderUnit>());

		SelectGrid->AddUnit(FolderUnitList[folderUnitInUse]);
		FolderUnitList[folderUnitInUse]->ChangeImage(folderContents[i].IsFolder);
		FolderUnitList[folderUnitInUse]->ChangeText(folderContents[i].name);
		folderUnitInUse++;		
	}	
}

void QFAUIEditorFileExplorer::UpdateCppItemList()
{
	QFAClassInstance* instance = (QFAClassInstance*)QFAEngineClassInstance::GetGameClassInstance();
	if (!instance)
		return;

	std::vector<QFAClass*>& classList = instance->GetClassList();
	CppItemList->RemoveAllUnit();
	CppUnitInUse = 0;
	
	for (int i = QFAObjectClasses::QOCMAX; i < classList.size(); i++)
	{
		if (CppUnitInUse == CppUnitList.size())
			CppUnitList.push_back(NewUI<QFAEditorExplorerFolderUnit>());
		
		CppItemList->AddUnit(CppUnitList[CppUnitInUse]);
		CppUnitList[CppUnitInUse]->ChangeImage(false);
		CppUnitList[CppUnitInUse]->ChangeText(QFAString::CharsTo32Chars(classList[i]->GetName()));
		CppUnitInUse++;
	}
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

void QFAUIEditorFileExplorer::SetNextFolderButton()
{	
	auto fun = [this](QFAUIUnit* unit)
	{
		if (CurentPathHistoryIndex + 1 >= PathHistory.size() || !FileExplorerSelected)
			return;

		CurentPathHistoryIndex++;
		CurentFolder = PathHistory[CurentPathHistoryIndex];
		BackButton->SetTextColor(ButoonOnColor);
		if (CurentPathHistoryIndex + 1 >= PathHistory.size())
			ForwardButton->SetTextColor(ButoonOffColor);
		else
			ForwardButton->SetTextColor(ButoonOnColor);

		PathChanged();
		if (FolderItemListSelectUnit->IsValid())
		{
			FolderItemListSelectUnit->SetBackgroundColor(OutFocusUnitColor);
			FolderItemListSelectUnit = nullptr;
		}
	};

	Events.SetForwardMouseDown(fun);
	ForwardButton->Events.SetLeftMouseDown(fun);
}

void QFAUIEditorFileExplorer::SetExplorerButtonE()
{
	ExplorerButton->Events.SetLeftMouseDown([this](QFAUIUnit* unit)
	{
		if (!FileExplorerSelected)
		{
			FileExplorerSelected = true;
			CppCanvas->SetEnable(false);
			FileExplorerMiddle->SetEnable(true);
			FileExplorerTop->SetEnable(true);
			UpdateFolderItemList();
			PathChanged();

			ExplorerButton->SetTextColor(ButoonOnColor);
			CppButton->SetTextColor(ButoonOffColor);

			SetWidth(StrWidth, ParentSetWidthMinus);
			SetHeight(StrHeight, ParentSetHeightMinus);
			SetTop(StrTop);
			SetLeft(StrLeft);
		}
	});
}

void QFAUIEditorFileExplorer::SetCppButtonE()
{
	CppButton->Events.SetLeftMouseDown([this](QFAUIUnit* unit)
	{
		if (FileExplorerSelected)
		{
			FileExplorerSelected = false;
			CppCanvas->SetEnable(true);
			FileExplorerMiddle->SetEnable(false);
			FileExplorerTop->SetEnable(false);
			UpdateCppItemList();

			ExplorerButton->SetTextColor(ButoonOffColor);
			CppButton->SetTextColor(ButoonOnColor);

			SetWidth(StrWidth, ParentSetWidthMinus);
			SetHeight(StrHeight, ParentSetHeightMinus);
			SetTop(StrTop);
			SetLeft(StrLeft);
		}
	});	
}

void QFAUIEditorFileExplorer::SetPreviousFolderButton()
{
	auto fun = [this](QFAUIUnit* unit)
	{
		if(CurentPathHistoryIndex == 0 || !FileExplorerSelected)
			return;

		CurentPathHistoryIndex--;
		CurentFolder = PathHistory[CurentPathHistoryIndex];
		ForwardButton->SetTextColor(ButoonOnColor);
		if (CurentPathHistoryIndex == 0)
			BackButton->SetTextColor(ButoonOffColor);
		else
			BackButton->SetTextColor(ButoonOnColor);

		PathChanged();
		if (FolderItemListSelectUnit->IsValid())
		{
			FolderItemListSelectUnit->SetBackgroundColor(OutFocusUnitColor);
			FolderItemListSelectUnit = nullptr;
		}
	};

	Events.SetBackwardMouseDown(fun);
	BackButton->Events.SetLeftMouseDown(fun);
}

void QFAUIEditorFileExplorer::DropFiles(int path_count, const char* paths[])
{	
	QFAEditorFileStorage::DropFiles(CurentFolder.id, path_count, paths);
	UpdateFolderItemList();
}

void QFAUIEditorFileExplorer::NotifyMainEditorWindowDrag(QFAEditorExplorerFolderUnit* unit)
{
	if (!unit)
		return;

	if (FileExplorerSelected)
	{
		for (size_t i = 0; i < folderUnitInUse; i++)
			if (FolderUnitList[i] == unit)
				return DragFun(false, folderContents[i].id);
	} 
	else
	{
		std::vector<QFAClass*>& classList = ((QFAClassInstance*)QFAEngineClassInstance::GetGameClassInstance())->GetGameClassList();
		// need put in QFAEditorExplorerFolderUnit(*unit) class id and don't search here
		for (size_t i = 0; i < CppUnitInUse; i++)
			if (CppUnitList[i] == unit)
				return DragFun(true, classList[i]->GetId());
	}
}

void QFAUIEditorFileExplorer::PathChanged()
{
	PathText->SetText(CurentFolder.path);
	UpdateFolderItemList();
}

void QFAUIEditorFileExplorer::WidthChanged(int oldValue)
{
	CppCanvas->SetWidth(((QFAEditorCanvas*)CppCanvas)->StrWidth, ((QFAEditorCanvas*)CppCanvas)->ParentSetWidthMinus);
	FileExplorerTop->SetWidth(((QFAEditorList*)FileExplorerTop)->StrWidth, ((QFAEditorList*)FileExplorerTop)->ParentSetWidthMinus);
	FileExplorerMiddle->SetWidth(((QFAEditorCanvas*)FileExplorerMiddle)->StrWidth, ((QFAEditorCanvas*)FileExplorerMiddle)->ParentSetWidthMinus);
}

void QFAUIEditorFileExplorer::HeightChanged(int oldValue)
{
	CppCanvas->SetHeight(((QFAEditorCanvas*)CppCanvas)->StrHeight, ((QFAEditorCanvas*)CppCanvas)->ParentSetHeightMinus);
	FileExplorerTop->SetHeight(((QFAEditorList*)FileExplorerTop)->StrHeight, ((QFAEditorList*)FileExplorerTop)->ParentSetHeightMinus);
	FileExplorerMiddle->SetHeight(((QFAEditorCanvas*)FileExplorerMiddle)->StrHeight, ((QFAEditorCanvas*)FileExplorerMiddle)->ParentSetHeightMinus);
	FileExplorerBottom->SetHeight(((QFAEditorList*)FileExplorerBottom)->StrHeight, ((QFAEditorList*)FileExplorerBottom)->ParentSetHeightMinus);
}

void QFAUIEditorFileExplorer::TopChanged(int oldValue)
{	
	((QFAEditorCanvas*)CppCanvas)->ParentSetPosition_y = Position_y;
	((QFAEditorList*)FileExplorerTop)->ParentSetPosition_y = Position_y;
	((QFAEditorCanvas*)FileExplorerMiddle)->ParentSetPosition_y = Position_y;
	((QFAEditorList*)FileExplorerBottom)->ParentSetPosition_y = Position_y;

	CppCanvas->SetTop(((QFAEditorCanvas*)CppCanvas)->StrTop);
	FileExplorerTop->SetTop(((QFAEditorList*)FileExplorerTop)->StrTop);
	FileExplorerMiddle->SetTop(((QFAEditorCanvas*)FileExplorerMiddle)->StrTop);
	FileExplorerBottom->SetTop(((QFAEditorList*)FileExplorerBottom)->StrTop);
}

void QFAUIEditorFileExplorer::LeftChanged(int oldValue)
{
	((QFAEditorCanvas*)CppCanvas)->ParentSetPosition_x = Position_x;
	((QFAEditorList*)FileExplorerTop)->ParentSetPosition_x = Position_x;
	((QFAEditorCanvas*)FileExplorerMiddle)->ParentSetPosition_x = Position_x;
	((QFAEditorList*)FileExplorerBottom)->ParentSetPosition_x = Position_x;

	CppCanvas->SetLeft(((QFAEditorCanvas*)CppCanvas)->StrLeft);
	FileExplorerTop->SetLeft(((QFAEditorList*)FileExplorerTop)->StrLeft);
	FileExplorerMiddle->SetLeft(((QFAEditorCanvas*)FileExplorerMiddle)->StrLeft);
	FileExplorerBottom->SetLeft(((QFAEditorList*)FileExplorerBottom)->StrLeft);
}

float QFAUIEditorFileExplorer::UpdateInnerHeight()
{
	return Height;
}

float QFAUIEditorFileExplorer::UpdateInnerWidth()
{
	return Width;
}

