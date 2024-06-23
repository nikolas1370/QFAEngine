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
	static void* OldGameCodeModule;
	const char SourceFolder[7] = "Source";
	static QFAGameCodeFunctions* GameCodeAPIFunction;

	enum DllFile
	{
		None,
		GameCode,
		GameCode_HotReload_1,
		GameCode_HotReload_2
	};

	enum CompileStatus
	{
		OK,
		Error
	};

public:

private:
	static DllFile CurentDllFile;
	static bool DllWasCompiled;
	static const char* CopyDllFrom;
	static const char* CopyPdbFrom;
	static const char* CopyDllIn;
	static const char* CopyPdbIn;
	static const wchar_t* LoadDllPath; // default
	static const wchar_t* DllHotPath;
	static const wchar_t* DllHot_1Path;
	static const wchar_t* DllHot_2Path;
	static const wchar_t* PdbHot_1Path;
	static const wchar_t* PdbHot_2Path;

	static const wchar_t* HotFolder_1Path;
	static const wchar_t* HotFolder_2Path;


	static void CompileGameCode(void (*callback)(CompileStatus));
	
	static void (*CompileCallback)(QFAGameCode::CompileStatus);

	// if return fals not need replacement old objects
	static bool LoadCode();


	/* inside usage */
	// if return true Compile Succeed
	static bool Compile();

	static void ReplacementOldDllObject();

	/*
		from QFAGameCode::CopyDllFrom to DllFile
		return true if copy done
	*/
	static bool CopyDll(DllFile f);
};

