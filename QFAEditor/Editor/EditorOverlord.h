#pragma once

#include "ShaderCompiler/ShaderCompiler.h"
#include <thread>
#include <EditorFileStorage.h>
class QFAEditorMainWindow;
class QFAEditorOverlord
{
	static QFAShaderCompiler compiler;
	static QFAEditorMainWindow* MainWindow;
	static bool IsInit;
	static QFAEditorFileStorage Storage;
public:
	static void StartLife();
private:
	static bool InitializationDon;
	static bool NewLoadText;
	static std::u32string LoadText;
	static std::u32string LoadText_2;

	static void Init();
	// call from not main thread
	static void PrepareToWork();
	// QFAOverlord call 
	static void StartFrame();
	static void EndFrame();

	static std::thread* LoadThread;
};