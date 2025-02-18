#pragma once
#include <thread>
#include <EditorFileStorage.h>

class QFAEditorMainWindow;
class QFAEditorOverlord
{
	static QFAEditorMainWindow* MainWindow;
	static bool IsInit;
	static QFAEditorFileStorage Storage;

	static bool InitializationDon;
	static std::u32string LoadText;

	// call from not main thread
	static void PrepareToWork();
	// QFAOverlord call 
	static void StartFrame();
	static void EndFrame();

public:
	static void StartLife();
};



