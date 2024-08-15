#include "epch.h"
#include "OptionWindow.h"
#include "EngineClassesInterface.h"
#include <EditorUI/ExplorerFolderUnit.h>
#include <Tools/String.h>

QFAEditorOptionWindow::QFAEditorOptionWindow(std::function<void(std::u32string fileName)> doneFun, std::function<void()> cancelFun)
{
	Window = QFAEditorWindow::CreateEngineWindow(600, 300, "Save level as", true, true, [this, cancelFun]()
	{
		Window = nullptr;
		if(cancelFun)
			cancelFun();

		delete this;
	});
	
	QFAViewport* viewport = Window->GetViewport(0);
	viewport->AddUnit(&Path);
	Path.SetTextSize(25);
	Path.SetHeight("30");

	viewport->AddUnit(&FolderList);
	FolderList.SetTop("30");
	FolderList.SetScrollType(QFAUIScroll::STVertical);
	FolderList.SetPositionType(QFAUIGrid::UPTAuto);
	FolderList.SetMin(100);
	FolderList.SetOffsets(10, 10);
	FolderList.SetRation(1.5);
	FolderList.FocusColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	FolderList.SelectColor = QFAColor(14, 134, 255, 255).GetColorF();
	FolderList.SelectLostFocusColor = QFAColor(64, 87, 111, 255).GetColorF();
	FolderList.SetWidth("100%");
	FolderList.SetHeight("100%");

	viewport->AddUnit(&FileNameInput);
	FileNameInput.SetTextSize(25);
	FileNameInput.SetTop("100% - 30");
	FileNameInput.SetHeight("30");
	FileNameInput.SetWidth("100% - 160");
	FileNameInput.SetBackgroundColor(QFAColor(20, 20, 20).GetColorF());

	Dane.SetText(U"Save");
	Dane.SetTextAlign(QFAText::ETextAlign::TACenter);
	Dane.SetTextSize(25);
	Dane.SetTop("100% - 30");
	Dane.SetLeft("100% - 75");
	Dane.SetWidth("70");
	viewport->AddUnit(&Dane);

	Cancel.SetText(U"Cancel");
	Cancel.SetTextAlign(QFAText::ETextAlign::TACenter);
	Cancel.SetTextSize(25);
	Cancel.SetTop("100% - 30");
	Cancel.SetLeft("100% - 155");
	Cancel.SetWidth("70");
	viewport->AddUnit(&Cancel);	

	FolderList.SelectEvent.DobleClick = [this](QFAUIParent* unitUnderCursore, void* extraData)
	{
		if(extraData)
			CurentPath = ((QFAFileSystem::FolderUnit*)extraData)->path;
		else
			CurentPath = std::filesystem::path(CurentPath).parent_path().u32string();

		UpdateList();
	}; 

	FileNameInput.SetTextChange([this]() { ChangeFilePath(); });
	Dane.Events.SetLeftMouseDownUp([this, doneFun](QFAUIUnit* unit)
	{			
		doneFun(FinalPath);
		delete this;
	});

	Cancel.Events.SetLeftMouseDownUp([this, cancelFun](QFAUIUnit* unit)
	{
		cancelFun();
		delete this;
	});

	CurentPath = U"Content";
	UpdateList();
}

QFAEditorOptionWindow::~QFAEditorOptionWindow()
{
	FolderList.RemoveAllUnit(true);
	if (Window)
	{
		Window->Close();
		((QFAEditorWindow*)Window)->SetClosedFun(nullptr);
	}
}

void QFAEditorOptionWindow::UpdateList()
{
	FolderContents.clear();
	FolderList.RemoveAllUnit(true);
	QFAFileSystem::GetFolderContents(CurentPath, FolderContents);

	std::filesystem::path parentPath = std::filesystem::path(CurentPath).parent_path();
	if(!parentPath.empty())
	{
		QFAEditorExplorerFolderUnit* fu = NewUI<QFAEditorExplorerFolderUnit>();
		fu->ChangeImage(true);
		fu->ChangeText(U"...");
		FolderList.AddUnit(fu, nullptr); // if extraData null it's mean need move back in path
	}

	for (size_t i = 0; i < FolderContents.size(); i++)
	{
		if (!FolderContents[i].IsFolder)
			break;

		QFAEditorExplorerFolderUnit* fu = NewUI<QFAEditorExplorerFolderUnit>();
		fu->ChangeImage(true);
		fu->ChangeText(FolderContents[i].name);
		FolderList.AddUnit(fu, &FolderContents[i]);
	}

	ChangeFilePath();
}

void QFAEditorOptionWindow::ChangeFilePath()
{
	FinalPath = CurentPath + U"/" + FileNameInput.GetValue() + U".qfa";
	Path.SetText(FinalPath);
}
