#pragma once
#include <filesystem>
#include <Object/Object.h>

class QFAEditorMainWindow;
class QFAEditorOverlord;
class QFAGameCodeFunctions;
class QFAUIEditorFileExplorer;
class QFAGameCode
{
	friend QFAEditorMainWindow;
	friend QFAEditorOverlord;
	friend QFAUIEditorFileExplorer;
	static void* GameCodeModule;// GameCodeModule is HMODULE, don't wanna include windows.h
	const char SourceFolder[7] = "Source";
	static QFAGameCodeFunctions* GameCodeAPIFunction;

	enum CompileStatus
	{
		OK,
		Error
	};

	static const char* CopyDllFrom;
	static const char* CopyDllIn;

	static void CompileGameCode(void (*callback)(CompileStatus));
	
	static void (*CompileCallback)(QFAGameCode::CompileStatus);

	static void LoadCode();

	/* inside usage */
	// if return true Compile Succeed
	static bool Compile();
};

