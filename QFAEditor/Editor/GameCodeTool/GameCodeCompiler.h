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

	static void CompileGameCode(void (*callback)(CompileStatus));
	



	static void (*CompileCallback)(GameCodeCompiler::CompileStatus);
	static void AnalyzeFile(std::filesystem::path filePath);
};

