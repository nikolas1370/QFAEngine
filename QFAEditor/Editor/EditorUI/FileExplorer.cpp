#include "epch.h"
#include "FileExplorer.h"
#include <EngineStuff/VulkanSuff.h>
#include <UI/UIList.h>
#include <UI/Canvas.h>
#include <UI/Text.h>
#include <UI/Grid.h>
#include <UI/Scroll.h>
#include <Tools/File/FileSystem.h>
#include "ExplorerFolderUnit.h"
#include <EditorWindows/MainEditorWindow.h>
#include <Overlord/Time.h>
#include <filesystem>
#include <EditorWindows/FileViewWindow.h>
#include <EditorFileStorage.h>
#include "../GameCodeTool/GameCodeCompiler.h"
#include <Tools/String.h>
#include <UI/SelectUnit.h>
#include <EngineClassesInterface.h>

QFAUIEditorFileExplorer::QFAUIEditorFileExplorer(QFAEngineWindow *window, std::function <void(bool isCppClass, size_t fileId)> dragFun)
{
	DragFun = dragFun;
	Window = window;
	Type = QFAUIType::CustomUnit;
	EditorType = QFAEditorUIType::FileExplorer;
	SetBackgroundColor(FileExplorerBackgroundColor);

	Events.SetForwardMouseDown(&QFAUIEditorFileExplorer::NextFolderButton, this);
	Events.SetBackwardMouseDown(&QFAUIEditorFileExplorer::PreviousFolderButton, this);

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
}

void QFAUIEditorFileExplorer::CreateTop()
{
	FileExplorerTop = new QFAUIList;
	FileExplorerTop->SetWidth("100%");
	FileExplorerTop->SetHeight("25");
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
	BackButton->SetTextColor(ButoonOffColor);
	ForwardButton->SetTextColor(ButoonOffColor);
	
	ForwardButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::NextFolderButton, this);
	BackButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::PreviousFolderButton, this);

	PathTextScroll = new QFAUIScroll; 
	PathTextScroll->SetWidth("100% - 50"); // 50 == FileExplorerTop->SetUnitWidth for SetUnitWidth + ForwardButton
	PathTextScroll->SetHeight("100%");
	PathTextScroll->SetScrollType(QFAUIScroll::STHorizon);

	PathText = new QFAText;
	PathTextScroll->SetUnit(PathText);
	PathText->SetTextSize(18);	
	PathText->SetOverflowWrap(QFAText::OWNone);
	
	FileExplorerTop->AddUnit(BackButton);
	FileExplorerTop->AddUnit(ForwardButton);
	FileExplorerTop->AddUnit(PathTextScroll);	
	AddHiddenChild(FileExplorerTop);
}

void QFAUIEditorFileExplorer::CreateMiddle()
{
	FileExplorerMiddle = new QFAUICanvas;
	FileExplorerMiddle->SetWidth("100%");
	FileExplorerMiddle->SetHeight("100% - 30 - 30");// (30 top) - (30 bottom)
	FileExplorerMiddle->SetTop("30");

	SelectGrid = new QFAUISelectGrid;

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

	SelectGrid->SelectEvent.LeftMouseDown = ([this](QFAUIParent* unit)
		{
			NotifyMainEditorWindowDrag((QFAEditorExplorerFolderUnit*)unit);
		});

	SelectGrid->SelectEvent.DobleClick = ([this](QFAUIParent* unit)
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
						{
							std::cout << "Level load\n";
							return; // write load level
						}

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
	FileExplorerBottom = new QFAUIList;
	FileExplorerBottom->SetWidth("100%");
	FileExplorerBottom->SetHeight("30");
	FileExplorerBottom->SetTop("100% - 30");

	FileExplorerBottom->SetListType(QFAUIList::LTHorizon);
	FileExplorerBottom->SetUnitWidth(25);

	ExplorerButton = new QFAText;	
	CppButton = new QFAText;
	CppButton->SetLeft("20");
	ExplorerButton->SetLeft("10");
	ExplorerButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	CppButton->SetFont(QFAEditorMainWindow::GetIcomonFont());
	CppButton->SetText(std::u32string(&CppFileButtonIconCode, 1));
	ExplorerButton->SetText(std::u32string(&FolderButtonIconCode, 1));
	ExplorerButton->SetTextSize(30);
	CppButton->SetTextSize(30);
	ExplorerButton->SetTextAlign(QFAText::TACenter);
	CppButton->SetTextAlign(QFAText::TACenter);
	ExplorerButton->SetTextColor(ButoonOnColor); // Explorer show first
	CppButton->SetTextColor(ButoonOffColor);

	CppButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::CppButtonE, this);
	ExplorerButton->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::ExplorerButtonE, this);
	FileExplorerBottom->AddUnit(ExplorerButton);
	FileExplorerBottom->AddUnit(CppButton);
	
	AddHiddenChild(FileExplorerBottom);
}

void QFAUIEditorFileExplorer::CreateCppTop()
{
	CppCanvas = new QFAUICanvas;
	CppCanvas->SetWidth("100%");
	CppCanvas->SetHeight("100% - 30"); // 30 FileExplorerBottom.height
	CppItemList = new QFAUISelectGrid;

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

	CppItemList->SelectEvent.LeftMouseDown = ([this](QFAUIParent* unit)
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
			FolderUnitList.push_back(new QFAEditorExplorerFolderUnit);

		SelectGrid->AddUnit(FolderUnitList[folderUnitInUse]);
		FolderUnitList[folderUnitInUse]->ChangeImage(folderContents[i].IsFolder);
		FolderUnitList[folderUnitInUse]->ChangeText(folderContents[i].name);
		folderUnitInUse++;		
	}	
}

void QFAUIEditorFileExplorer::UpdateCppItemList()
{
	if (!QFAGameCode::GetAPI())
		return;

	std::vector<QFAClass*>& classList = QFAGameCode::GetAPI()->GetGameClassList();
	CppItemList->RemoveAllUnit();
	CppUnitInUse = 0;
	
	for (size_t i = 0; i < classList.size(); i++)
	{
		if (CppUnitInUse == CppUnitList.size())
			CppUnitList.push_back(new QFAEditorExplorerFolderUnit);
		
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

void QFAUIEditorFileExplorer::NextFolderButton(QFAUIUnit* unit, void* _this)
{	
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;

	if (thisUnit->CurentPathHistoryIndex + 1 >= thisUnit->PathHistory.size() || !thisUnit->FileExplorerSelected)
		return;

	thisUnit->CurentPathHistoryIndex++;
	thisUnit->CurentFolder = thisUnit->PathHistory[thisUnit->CurentPathHistoryIndex];
	thisUnit->BackButton->SetTextColor(thisUnit->ButoonOnColor);
	if (thisUnit->CurentPathHistoryIndex + 1 >= thisUnit->PathHistory.size())
		thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOffColor);
	else
		thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOnColor);

	thisUnit->PathChanged();
	if(thisUnit->FolderItemListSelectUnit->IsValid())
	{
		thisUnit->FolderItemListSelectUnit->SetBackgroundColor(thisUnit->OutFocusUnitColor);
		thisUnit->FolderItemListSelectUnit = nullptr;
	}
}

void QFAUIEditorFileExplorer::ExplorerButtonE(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;
	if (!thisUnit->FileExplorerSelected)
	{
		thisUnit->FileExplorerSelected = true;
		thisUnit->CppCanvas->SetEnable(false);
		thisUnit->FileExplorerMiddle->SetEnable(true);
		thisUnit->FileExplorerTop->SetEnable(true);
		thisUnit->UpdateFolderItemList();
		thisUnit->PathChanged();

		thisUnit->ExplorerButton->SetTextColor(thisUnit->ButoonOnColor);
		thisUnit->CppButton->SetTextColor(thisUnit->ButoonOffColor);

		thisUnit->SetWidth(thisUnit->StrWidth, thisUnit->ParentSetWidthMinus);
		thisUnit->SetHeight(thisUnit->StrHeight, thisUnit->ParentSetHeightMinus);
		thisUnit->SetTop(thisUnit->StrTop);
		thisUnit->SetLeft(thisUnit->StrLeft);
	}
}

void QFAUIEditorFileExplorer::CppButtonE(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;
	if (thisUnit->FileExplorerSelected)
	{
		thisUnit->FileExplorerSelected = false;
		thisUnit->CppCanvas->SetEnable(true);
		thisUnit->FileExplorerMiddle->SetEnable(false);
		thisUnit->FileExplorerTop->SetEnable(false);
		thisUnit->UpdateCppItemList();

		thisUnit->ExplorerButton->SetTextColor(thisUnit->ButoonOffColor);
		thisUnit->CppButton->SetTextColor(thisUnit->ButoonOnColor);

		thisUnit->SetWidth(thisUnit->StrWidth, thisUnit->ParentSetWidthMinus);
		thisUnit->SetHeight(thisUnit->StrHeight, thisUnit->ParentSetHeightMinus);
		thisUnit->SetTop(thisUnit->StrTop);
		thisUnit->SetLeft(thisUnit->StrLeft);
	}	
}

void QFAUIEditorFileExplorer::PreviousFolderButton(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)_this;	
	if (thisUnit->CurentPathHistoryIndex == 0 || !thisUnit->FileExplorerSelected)
		return;

	thisUnit->CurentPathHistoryIndex--;
	thisUnit->CurentFolder = thisUnit->PathHistory[thisUnit->CurentPathHistoryIndex];
	thisUnit->ForwardButton->SetTextColor(thisUnit->ButoonOnColor);
	if(thisUnit->CurentPathHistoryIndex == 0)
		thisUnit->BackButton->SetTextColor(thisUnit->ButoonOffColor);
	else
		thisUnit->BackButton->SetTextColor(thisUnit->ButoonOnColor);

	thisUnit->PathChanged();
	if (thisUnit->FolderItemListSelectUnit->IsValid())
	{
		thisUnit->FolderItemListSelectUnit->SetBackgroundColor(thisUnit->OutFocusUnitColor);
		thisUnit->FolderItemListSelectUnit = nullptr;
	}
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
		std::vector<QFAClass*>& classList = QFAGameCode::GetAPI()->GetGameClassList();
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

