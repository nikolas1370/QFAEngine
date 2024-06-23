#pragma once
#include <Render/UI/UIParentHiddenChild.h>
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

	QFAUIList* FileExplorerTop;
	QFAText* BackButton;
	QFAText* ForwardButton;
	QFAUIScroll* PathTextScroll;
	QFAText* PathText;
	static const char32_t BackButtonIconCode = 59972;		// Icomon Free (circle-right) https://icomoon.io/
	static const char32_t ForwardButtonIconCode = 59970;	// Icomon Free (circle-left)
	static const char32_t FolderButtonIconCode = 59651;		// Hawcons (folder1)
	static const char32_t CppFileButtonIconCode = 59656;	// Hawcons (document-file-cpp)

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
protected:
	void MySlotChange(QFAUIUnit* unit) override {} ;
	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	QFAWindow* Window;
public:
	QFAUIEditorFileExplorer(QFAWindow* window, std::function <void (bool isCppClass, size_t id)> dragFun);
	~QFAUIEditorFileExplorer();

private:
	std::function <void(bool isCppClass, size_t id)> DragFun;
	/* FolderUnitList and folderContents[0] have same order */
	std::vector<QFAEditorExplorerFolderUnit*> FolderUnitList; 	
	std::vector<QFAFileSystem::FolderUnit> folderContents;

	std::vector<QFAEditorExplorerFolderUnit*> CppUnitList;
	int folderUnitInUse = 0;
	int CppUnitInUse = 0;

	QFAEditorExplorerFolderUnit* FolderItemListInFocusUnit = nullptr;
	QFAEditorExplorerFolderUnit* FolderItemListSelectUnit = nullptr;
	void CreateTop();
	void CreateMiddle();
	void CreateBottom();
	void CreateCppTop();

	void UpdateFolderItemList();
	void UpdateCppItemList();
	
	void PathChanged();
	void NextFolder(QFAFileSystem::FolderUnit nextFolder);
	static void NextFolderButton(QFAUIUnit* unit , void* _this);
	static void PreviousFolderButton(QFAUIUnit* unit, void* _this);
	static void ExplorerButtonE(QFAUIUnit* unit, void* _this);
	static void CppButtonE(QFAUIUnit* unit, void* _this);

	std::vector<QFAFileSystem::FolderUnit> PathHistory;
	int CurentPathHistoryIndex = 0;
	const std::u32string MainFolder = U"Content";

	const QFAColorF FileExplorerBackgroundColor = QFAColor(36, 36, 36, 255).GetColorF();
	const QFAColorF ButoonOffColor = QFAColor(106, 106, 106).GetColorF();
	const QFAColorF ButoonOnColor = QFAColor(255, 255, 255).GetColorF();
	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	const QFAColorF OutFocusUnitColor = QFAColorF(0.0f, 0.0f, 0.0f, 0.0f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();
	const QFAColorF SelectUnitNotFocus = QFAColor(64, 87, 111, 255).GetColorF();

	void DropFiles(int path_count, const char* paths[]);

	QFAEditorFileViewWindow* FileViewWindow = nullptr;

	void NotifyMainEditorWindowDrag(QFAEditorExplorerFolderUnit* unit);
};