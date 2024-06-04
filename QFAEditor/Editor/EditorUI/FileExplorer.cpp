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

#include <EditorFileTypes.h>
#include <EditorWindows/FileViewWindow.h>
#include <EditorFileStorage.h>
#include "../GameCodeTool/GameCodeCompiler.h"
#include <Tools/String.h>

QFAUIEditorFileExplorer::QFAUIEditorFileExplorer(QFAWindow *window, std::function <void(size_t fileId)> dragFun)
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
	BackButton->SetTextColor(ButoonOffColor);
	ForwardButton->SetTextColor(ButoonOffColor);
	
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
	AddHiddenChild(FileExplorerTop);
}

void QFAUIEditorFileExplorer::CreateMiddle()
{
	FileExplorerMiddle = new QFAUICanvas;

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
	FileExplorerMiddle->AddUnit(FolderItemListScroll);

	FolderItemList->Events.SetInFocus(&QFAUIEditorFileExplorer::FolderItemListInFocus, this);
	FolderItemList->Events.SetOutFocus(&QFAUIEditorFileExplorer::FolderItemListOutFocus, this);
	FolderItemList->Events.SetLeftMouseDown(&QFAUIEditorFileExplorer::FolderItemListLeftMouseDown, this);
	AddHiddenChild(FileExplorerMiddle);
}

void QFAUIEditorFileExplorer::CreateBottom()
{
	FileExplorerBottom = new QFAUIList;
	FileExplorerBottom->SetListType(QFAUIList::LTHorizon);
	FileExplorerBottom->SetUnitWidth(25);

	ExplorerButton = new QFAText;
	CppButton = new QFAText;
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

	QFAUISlot::SListSlot slot;
	slot.marginLeft = 10;
	ExplorerButton->SetSlot(&slot);	
	CppButton->SetSlot(&slot);
	
	AddHiddenChild(FileExplorerBottom);
}

void QFAUIEditorFileExplorer::CreateCppTop()
{
	CppCanvas = new QFAUICanvas;

	CppListScroll = new QFAUIScroll;
	CppListScroll->SetScrollType(QFAUIScroll::STVertical);

	CppItemList = new QFAUIGrid;
	CppListScroll->SetUnit(CppItemList);
	CppItemList->SetPositionType(QFAUIGrid::UPTAuto);
	CppItemList->SetMin(100);

	CppItemList->SetOffsets(10, 10);
	CppItemList->SetRation(1.5);

	QFAUISlot::SCanvasSlot slot;
	slot.Height = 1.0f;
	slot.Width = 1.0f;
	slot.x = 0.0f;
	slot.y = 0.0f;
	CppListScroll->SetSlot(&slot);
	CppCanvas->AddUnit(CppListScroll);
	CppCanvas->SetEnable(false);
	AddHiddenChild(CppCanvas);
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
			FolderUnitList.push_back(new QFAEditorExplorerFolderUnit);

		FolderItemList->AddUnit(FolderUnitList[folderUnitInUse]);
		FolderUnitList[folderUnitInUse]->ChangeImage(folderContents[i].IsFolder);
		FolderUnitList[folderUnitInUse]->ChangeText(folderContents[i].name);
		folderUnitInUse++;		
	}	
}

void QFAUIEditorFileExplorer::UpdateCppItemList()
{
	if (!QFAGameCode::GameCodeAPIFunction)
		return;

	std::vector<QFAClassInfoBase*>* classList = QFAGameCode::GameCodeAPIFunction->GetGameClassList();
	QFAClassInfoBase** classInfo = classList->data();
	CppItemList->removeAllUnit();
	CppUnitInUse = 0;
	for (size_t i = 0; i < classList->size(); i++)
	{
		if (CppUnitInUse == CppUnitList.size())
			CppUnitList.push_back(new QFAEditorExplorerFolderUnit);
		
		CppItemList->AddUnit(CppUnitList[CppUnitInUse]);
		CppUnitList[CppUnitInUse]->ChangeImage(false);
		CppUnitList[CppUnitInUse]->ChangeText(QFAString::CharsTo32Chars(classInfo[i]->GetClassName()));
		CppUnitInUse++;
	}
}

void QFAUIEditorFileExplorer::FolderItemListLeftMouseDown(QFAUIUnit* unit, void* _this)
{
	QFAUIEditorFileExplorer* thisUnit = (QFAUIEditorFileExplorer*)(_this);

	if (unit == thisUnit->FolderItemListSelectUnit)
		return thisUnit->NotifyMainEditorWindowDrag((QFAEditorExplorerFolderUnit*)unit);

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
			thisUnit->NotifyMainEditorWindowDrag((QFAEditorExplorerFolderUnit*)parent);
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

				thisUnit->InputFocus = true;
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

		thisUnit->ChangeSize((unsigned int)thisUnit->Width, (unsigned int)thisUnit->Height);
		thisUnit->ChangePosition((int)thisUnit->Position_x, (int)thisUnit->Position_y);
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

		thisUnit->ChangeSize((unsigned int)thisUnit->Width, (unsigned int)thisUnit->Height);
		thisUnit->ChangePosition((int)thisUnit->Position_x, (int)thisUnit->Position_y);				
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

	for (size_t i = 0; i < folderUnitInUse; i++)
		if (FolderUnitList[i] == unit)
			return DragFun(folderContents[i].id);
}

void QFAUIEditorFileExplorer::LostInputFocus()
{	
	if (FolderItemListSelectUnit->IsValid())
		FolderItemListSelectUnit->SetBackgroundColor(SelectUnitNotFocus);
	
	InputFocus = false;
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
	Width = w;
	Height = h;
	SetChildSize(CppCanvas, w, h - FileExplorerBottomHeight);
	SetChildSize(FileExplorerTop, w, h);
	SetChildSize(FileExplorerMiddle, w, h >= FileExplorerTopHeight ? (h - FileExplorerTopHeight - FileExplorerBottomHeight) : 0);
	SetChildSize(FileExplorerBottom, w, FileExplorerBottomHeight);

}

void QFAUIEditorFileExplorer::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(CppCanvas, x, y);
	SetChildPosition(FileExplorerTop, x, y );
	SetChildPosition(FileExplorerMiddle, x, y + FileExplorerTopHeight);
	SetChildPosition(FileExplorerBottom, x, y + FileExplorerTopHeight + FileExplorerMiddle->GetSize().Y);	
}

float QFAUIEditorFileExplorer::UpdateInnerHeight()
{
	return Height;
}

float QFAUIEditorFileExplorer::UpdateInnerWidth()
{
	return Width;
}