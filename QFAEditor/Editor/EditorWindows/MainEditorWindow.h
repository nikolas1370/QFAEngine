#pragma once
#include <Render/UI/Text.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>

class QFAWindow;
class QFAUIEditorFileExplorer;
class QFAUICanvas;
class QFAEditorOverlord;
class QFAUIList;
class ACameraEditor;
class QFAEditorGameViewportInfo;
class QFAEditorMainWindow
{
	friend QFAEditorOverlord;

	static QFAEditorMainWindow* MainWindow;
	QFAWindow* Window;
	QFAViewport* GameViewport;

	QFAUICanvas* LoadCanvas;
	QFAUIList* TextList;
	QFAText* Text;
	QFAText* LoadText;
	QFAText* LoadText_2;
	QFAText* LoadText_3;
	/**/
	QFAUICanvas* WindowCanvas = nullptr;
	QFAUIEditorFileExplorer* FileExplorer;

	QFAEditorGameViewportInfo* GameViewportInfo;

	static QFAText::SFont* Icomonfont;
	static const int LoaderWidth = 500;
	static const int LoaderHeight = 200;
	static const int WorkWidth = 1000;
	static const int WorkHeight = 600;

	QFAInput* Input;

	/*
		Worlds[0] == EditorWorld
		Worlds[1] == GameWorld
	*/
	QWorld* Worlds = nullptr; // allocated with new[]
	ACameraEditor* EditorCamera = nullptr;
public:
	QFAEditorMainWindow();
	~QFAEditorMainWindow();

	inline static QFAText::SFont* GetIcomonFont()
	{
		return Icomonfont;
	}
private:
	enum EFocus
	{
		FNone,
		FActorList,
		FFileExplorer
	};

	EFocus Focus = EFocus::FNone;

	void CreateLoadUI();
	void ChangeLoadInfo(std::u32string text, std::u32string text_2);
	// call in QFAEditorOverlord 
	void CreateUI();

	size_t CurentDragFileId = 0;
	static void StartDragAndDrop(size_t fileId);
	static void EndDragAndDrop(EKey::Key key);
	void PickMesh(EKey::Key key);
	void PrepareGameViewport();
	void PrepareCallback();

	void AddActorToWorlds(QActor* actor, SEditorFile& ef);

	QFAInput* input;
	FVector2D PickObjectLastCursorPos;

	bool LeftCTRLPress = false;
	static void GameCompileCallback(GameCodeCompiler::CompileStatus status);
	bool CompileStarted = false;
};

