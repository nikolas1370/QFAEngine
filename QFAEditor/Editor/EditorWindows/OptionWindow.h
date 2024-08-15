#pragma once
#include <UI/SelectUnit.h>
#include <UI/TextInput.h>
#include <Tools/File/FileSystem.h>

class QFAEngineWindow;
class QFAEditorOptionWindow
{
	QFAEngineWindow* Window = nullptr;
	QFAText Path;
	QFAUISelectGrid FolderList;
	QFAUITextInput FileNameInput;
	QFAText Dane;
	QFAText Cancel;

	std::u32string CurentPath;
	std::u32string FinalPath;
	std::vector<QFAFileSystem::FolderUnit> FolderContents;
	void UpdateList();
	void ChangeFilePath();
public:
	QFAEditorOptionWindow(std::function<void(std::u32string fileName)> doneFun, std::function<void()> cancelFun);
	~QFAEditorOptionWindow();

	inline bool IsOpen()
	{
		return (bool)Window;
	}

private:

};
