#pragma once
#include <filesystem>


class QFAEditorMainWindow;
class QFAEditorOverlord;
class QFAGameCode
{
	friend QFAEditorMainWindow;
	friend QFAEditorOverlord;
	static void* GameCodeModule;// GameCodeModule is HMODULE, don't wanna include windows.h
	const char SourceFolder[7] = "Source";
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

