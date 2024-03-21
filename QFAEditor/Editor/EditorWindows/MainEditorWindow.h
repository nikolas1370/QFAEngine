#pragma once
#include <Render/UI/Text.h>


class QFAWindow;
class QFAUIEditorFileExplorer;
class QFAUICanvas;
class QFAEditorOverlord;
class QFAUIList;
class QFAEditorMainWindow
{
	friend QFAEditorOverlord;

	static QFAEditorMainWindow* MainWindow;
	QFAWindow* Window;

	QFAUICanvas* LoadCanvas;
	QFAUIList* TextList;
	QFAText* Text;
	QFAText* LoadText;
	QFAText* LoadText_2;
	QFAText* LoadText_3;
	/**/
	QFAUICanvas* WindowCanvas = nullptr;
	QFAUIEditorFileExplorer* FileExplorer;

	static QFAText::SFont* Icomonfont;
	static const int LoaderWidth = 500;
	static const int LoaderHeight = 200;
	static const int WorkWidth = 1000;
	static const int WorkHeight = 600;
public:
	QFAEditorMainWindow();
	~QFAEditorMainWindow();

	inline static QFAText::SFont* GetIcomonFont()
	{
		return Icomonfont;
	}
private:

	void CreateLoadUI();
	void ChangeLoadInfo(std::u32string text, std::u32string text_2);
	// call in QFAEditorOverlord 
	void CreateUI();
	void DropFun(int path_count, const char* paths[]);



};

