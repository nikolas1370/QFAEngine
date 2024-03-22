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
class QFAUIEditorFileExplorer : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;

	QFAUIList* FileExplorerTop;
	QFAText* BackButton;
	QFAText* ForwardButton;
	QFAUIScroll* PathTextScroll;
	QFAText* PathText;
	
	static const char32_t BackButtonIconCode = 61840;// f190
	static const char32_t ForwardButtonIconCode = 61838;// f18e

	QFAUICanvas* FileExplorerBottom;
	QFAUIGrid* FolderItemList;
	QFAUIScroll* FolderItemListScroll;

	unsigned int FileExplorerTopHeight = 20;
	QFAFileSystem::FolderUnit CurentFolder;
protected:
	void MySlotChange(QFAUIUnit* unit) override;
	void ChangeSize(unsigned int w, unsigned int h) override;
	void ChangePosition(int x, int y) override;
	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	QFAWindow* Window;
public:
	QFAUIEditorFileExplorer(QFAWindow* window, std::function <void (size_t fileId)> dragFun);
	~QFAUIEditorFileExplorer();

private:
	std::function <void(size_t fileId)> DragFun;
	/* FolderUnitList and folderContents[0] have same order */
	std::vector<QFAEditorExplorerFolderUnit*> FolderUnitList; 
	std::vector<QFAFileSystem::FolderUnit> folderContents;
	int folderUnitInUse = 0;

	QFAEditorExplorerFolderUnit* FolderItemListInFocusUnit = nullptr;
	QFAEditorExplorerFolderUnit* FolderItemListSelectUnit = nullptr;
	void CreateTop();
	void CreateBottom();

	static void FolderItemListInFocus(QFAUIUnit* unit, void* _this);
	static void FolderItemListOutFocus(void* _this);
	static void FolderItemListLeftMouseDown(QFAUIUnit* unit, void* _this);
	


	void UpdateFolderItemList();

	const uint64_t MouseDownMaxTimeInMS = 400;
	const uint64_t MouseDownMaxTime = MouseDownMaxTimeInMS * 10000;// convert from ms to 1/10 microseconds
	uint64_t LastLeftMouseDownTime;

	void PathChanged();
	void NextFolder(QFAFileSystem::FolderUnit nextFolder);
	static void NextFolderButton(QFAUIUnit* unit , void* _this);
	static void PreviousFolderButton(QFAUIUnit* unit, void* _this);

	std::vector<QFAFileSystem::FolderUnit> PathHistory;
	int CurentPathHistoryIndex = 0;
	const std::u32string MainFolder = U"Content";

	const QFAColorF FileExplorerBackgroundColor = QFAColor(36, 36, 36, 255).GetColorF();
	const QFAColorF ButoonOffColor = QFAColor(106, 106, 106).GetColorF();
	const QFAColorF ButoonOnColor = QFAColor(255, 255, 255).GetColorF();
	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	const QFAColorF OutFocusUnitColor = QFAColorF(0.0f, 0.0f, 0.0f, 0.0f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();

	void DropFiles(int path_count, const char* paths[]);







	QFAEditorFileViewWindow* FileViewWindow = nullptr;

	void NotifyMainEditorWindowDrag(QFAEditorExplorerFolderUnit* unit);
};
