#pragma once
#include <UI/Text.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>
#include <Input/Input.h>
#include <Object/World/World.h>
#include <EngineClassesInterface.h>

class QEditorWorld : public QWorld
{
	friend QFAEditorMainWindow;
};

class QFAEngineWindow;
class QFAUIEditorFileExplorer;
class QFAUICanvas;
class QFAEditorOverlord;
class QFAUIList;
class ACameraEditor;
class QFAEditorGameViewportInfo;
class QFAViewportHolder;
class QFAEditorMainWindow
{
	friend QFAEditorOverlord;
	enum EFocus
	{
		FNone,
		FActorList,
		FFileExplorer
	};

	const int LoaderWidth = 500;
	const int LoaderHeight = 200;
	const int WorkWidth = 1000;
	const int WorkHeight = 600;

	static QFAEditorMainWindow* MainWindow;
	static QFAText::SFont* Icomonfont;
	

	QFAEngineWindow* Window;
	QFAEditorViewportHolder* GameViewport;

	QFAUICanvas* LoadCanvas;
	QFAUIList* TextList;
	QFAText* Text;
	QFAText* LoadText;
	QFAText* LoadText_2;

	QFAUICanvas* WindowCanvas = nullptr;
	QFAUIEditorFileExplorer* FileExplorer;

	QFAEditorGameViewportInfo* GameViewportInfo;

	QFAInput* Input;
	EFocus Focus = EFocus::FNone;
	bool IsCppClass; // if false it's file(QFAEditorFileTypes) 
	size_t CurentDragId = 0;// file or cpp class
	FVector2D PickObjectLastCursorPos;

	bool LeftCTRLPress = false;
	bool CompileStarted = false;
	/*
		Worlds[0] == EditorWorld
		Worlds[1] == GameWorld
	*/
	QWorld* Worlds = nullptr; // allocated with new[]
	ACameraEditor* EditorCamera = nullptr;

	static void StartDragAndDrop(bool isCppClass, size_t id);
	static void EndDragAndDrop(EKey::Key key);
	static void GameCompileCallback(QFAGameCode::CompileStatus status);

public:
	inline static QFAText::SFont* GetIcomonFont()
	{
		return Icomonfont;
	}

private:

	void CreateLoadUI();
	void ChangeLoadInfo(std::u32string text, std::u32string text_2);
	// call in QFAEditorOverlord if initialization after start editor done
	void CreateMainEdirorUI();

	void PickMesh(EKey::Key key);
	void PrepareGameViewport();
	void PrepareCallback();

	void AddActorToWorlds(QActor* actor, std::u32string actorName, size_t id, bool isCppClass);

	void CreateInput();

public:
	QFAEditorMainWindow();
	~QFAEditorMainWindow();
};