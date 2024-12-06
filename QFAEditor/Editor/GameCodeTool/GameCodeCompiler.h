#pragma once
#include <filesystem>
#include <Object/Class.h>

class QFAEditorMainWindow;
class QFAEditorOverlord;
class QFAUIEditorFileExplorer;
class QFAGameCode
{
	friend QFAEditorMainWindow;
	friend QFAEditorOverlord;

	const char SourceFolder[7] = "Source";
	static const char* CopyDllFrom;
	static const char* CopyPdbFrom;
	static const char* CopyDllIn;
	static const char* CopyPdbIn;
	static const wchar_t* LoadDllPath; // default

	static void* GameCodeModule;// GameCodeModule is HMODULE, don't wanna include windows.h

	// if return fals not need replacement old objects
	static bool LoadCode();
};

