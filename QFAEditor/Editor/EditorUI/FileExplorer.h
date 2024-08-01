#pragma once
#include <UI/UIParentHiddenChild.h>
#include <Tools/File/FileSystem.h>

class QFAUIList;
class QFAUICanvas;
class QFAUIGrid;
class QFAText;
class QFAEditorExplorerFolderUnit;
class QFAUIScroll;
class QFAEditorMainWindow;
class QFAEditorFileViewWindow;
class QFAUISelectGrid;
class QFAClassInfoBase;
class QFAUIEditorFileExplorer : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;

	const QFAColorF FileExplorerBackgroundColor = QFAColor(36, 36, 36, 255).GetColorF();
	const QFAColorF ButoonOffColor = QFAColor(106, 106, 106).GetColorF();
	const QFAColorF ButoonOnColor = QFAColor(255, 255, 255).GetColorF();
	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	const QFAColorF OutFocusUnitColor = QFAColorF(0.0f, 0.0f, 0.0f, 0.0f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();
	const QFAColorF SelectUnitNotFocus = QFAColor(64, 87, 111, 255).GetColorF();

	static const char32_t BackButtonIconCode = 59972;		// Icomon Free (circle-right) https://icomoon.io/
	static const char32_t ForwardButtonIconCode = 59970;	// Icomon Free (circle-left)
	static const char32_t FolderButtonIconCode = 59651;		// Hawcons (folder1)
	static const char32_t CppFileButtonIconCode = 59656;	// Hawcons (document-file-cpp)


	QFAUIList* FileExplorerTop;
	QFAText* BackButton;
	QFAText* ForwardButton;
	QFAUIScroll* PathTextScroll;
	QFAText* PathText;
	QFAUICanvas* FileExplorerMiddle;
	QFAUISelectGrid* SelectGrid;
	
	QFAUIList* FileExplorerBottom;
	QFAText* ExplorerButton;
	QFAText* CppButton;

	QFAUICanvas* CppCanvas;
	QFAUISelectGrid* CppItemList;
	
	bool FileExplorerSelected = true;

	unsigned int FileExplorerTopHeight = 20;
	unsigned int FileExplorerBottomHeight = 35;
	QFAFileSystem::FolderUnit CurentFolder;

	std::vector<QFAFileSystem::FolderUnit> PathHistory;
	int CurentPathHistoryIndex = 0;
	const std::u32string MainFolder = U"Content";

	std::function <void(bool isCppClass, size_t id)> DragFun;
	/* FolderUnitList and folderContents[0] have same order */
	std::vector<QFAEditorExplorerFolderUnit*> FolderUnitList;
	std::vector<QFAFileSystem::FolderUnit> folderContents;

	std::vector<QFAEditorExplorerFolderUnit*> CppUnitList;
	int folderUnitInUse = 0;
	int CppUnitInUse = 0;

	QFAEditorExplorerFolderUnit* FolderItemListInFocusUnit = nullptr;
	QFAEditorExplorerFolderUnit* FolderItemListSelectUnit = nullptr;
	QFAEditorFileViewWindow* FileViewWindow = nullptr;

protected:
	QFAEngineWindow* Window;

private:
	void CreateTop();
	void CreateMiddle();
	void CreateBottom();
	void CreateCppTop();

	void UpdateFolderItemList();
	void UpdateCppItemList();

	void PathChanged();
	void NextFolder(QFAFileSystem::FolderUnit nextFolder);
	void SetNextFolderButton();
	void SetPreviousFolderButton();
	void SetExplorerButtonE();
	void SetCppButtonE();

	void DropFiles(int path_count, const char* paths[]);
	void NotifyMainEditorWindowDrag(QFAEditorExplorerFolderUnit* unit);

protected:
	void MySlotChange(QFAUIUnit* unit) override {} ;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAUIEditorFileExplorer(QFAEngineWindow* window, std::function <void (bool isCppClass, size_t id)> dragFun);
	~QFAUIEditorFileExplorer();


};