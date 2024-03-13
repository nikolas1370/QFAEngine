#pragma once
#include <Render/UI/Text.h>

class QFAWindow;
class QFAUIEditorFileExplorer;
class QFAUICanvas;
class QFAEditorMainWindow
{
	static QFAEditorMainWindow* MainWindow;
	QFAWindow* Window;
	QFAUICanvas* WindowCanvas;
	QFAUIEditorFileExplorer* FileExplorer;

	static QFAText::SFont* Icomonfont;
public:
	QFAEditorMainWindow();
	~QFAEditorMainWindow();

	inline static QFAText::SFont* GetIcomonFont()
	{
		return Icomonfont;
	}
private:
	void CreateUI();
	void DropFun(int path_count, const char* paths[]);



};

