#pragma once
#include <filesystem>

class QFAEditorMainWindow;
class GameCodeCompiler
{
	friend QFAEditorMainWindow;
	const char SourceFolder[7] = "Source";
	enum CompileStatus
	{
		OK,
		Error
	};

	static const char* CopyDllFrom;
	static const char* CopyDllIn;

	static void CompileGameCode(void (*callback)(CompileStatus));
	
	static void (*CompileCallback)(GameCodeCompiler::CompileStatus);

	/* inside usage */
	// if return true Compile Succeed
	static bool Compile();
};

