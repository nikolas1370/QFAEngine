#pragma once
#include <Render/UI/Text.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>
#include <Input/Input.h>

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
	// call in QFAEditorOverlord if initialization after start editor done
	void CreateMainEdirorUI();

	bool IsCppClass; // if false it's file(QFAEditorFileTypes) 
	size_t CurentDragId = 0;// file or cpp class
	static void StartDragAndDrop(bool isCppClass, size_t id);
	static void EndDragAndDrop(EKey::Key key);
	void PickMesh(EKey::Key key);
	void PrepareGameViewport();
	void PrepareCallback();

	void AddActorToWorlds(QActor* actor, std::u32string actorName, size_t id, bool isCppClass);

	QFAInput* input;
	FVector2D PickObjectLastCursorPos;

	bool LeftCTRLPress = false;
	static void GameCompileCallback(QFAGameCode::CompileStatus status);
	bool CompileStarted = false;

	void CreateInput();
};

