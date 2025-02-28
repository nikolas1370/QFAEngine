﻿#pragma once
#include <UI/Text.h>
#include <EditorFileStorage.h>
#include <GameCodeTool/GameCodeCompiler.h>
#include <Input/Input.h>
#include <Object/World/World.h>
#include <EngineClassesInterface.h>
#include "OptionWindow.h"

class QFAEngineWindow;
class QFAUIEditorFileExplorer;
class QFAUICanvas;
class QFAEditorOverlord;
class QFAUIList;
class AFlyingCamera;
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
	
	QFAEditorOptionWindow* OptionWindow = nullptr;

	QFAEngineWindow* Window;
	QFAEditorViewportHolder* GameViewport = nullptr;

	QFAUICanvas* LoadCanvas = nullptr;
	QFAUIList* TextList = nullptr;
	QFAText* Text = nullptr;
	QFAText* LoadText = nullptr;
	QFAText* LoadText_2 = nullptr;

	QFAUICanvas* TopInWindowCanvas = nullptr;
	QFAText* RunButton = nullptr;

	QFAUICanvas* WindowCanvas = nullptr;
	QFAUIEditorFileExplorer* FileExplorer = nullptr;

	QFAEditorGameViewportInfo* GameViewportInfo = nullptr;

	static const char32_t StopSymbol = 59648;
	static const char32_t PlaySymbol = 59649;

	QFAInput* Input = nullptr;
	EFocus Focus = EFocus::FNone;
	bool IsCppClass; // if false it's file(QFAEditorFileTypes) 
	int CurentDragId = 0;// file or cpp class
	FVector2D PickObjectLastCursorPos;

	bool LeftCTRLPress = false;
	/*
		Worlds[0] == EditorWorld
		Worlds[1] == GameWorld
	*/
	std::array<QWorld*, 2> Worlds;
	QWorld* CurentWorld = nullptr;
	AFlyingCamera* EditorCamera = nullptr;

	QFAEditorLevel* Level = nullptr;

	static void StartDragAndDrop(bool isCppClass, size_t id);
	static void EndDragAndDrop(EKey::Key key);
	void LoadLevel(QFAEditorFileStorage::QFAContentFile& cf);

	void SetRunButton();
	void StartGame();
	void EndGame();

	void ChangeTitle(std::u32string levelPAth);
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

	void AddActorToWorlds(QActor* actor, std::u32string actorName, size_t id, bool isCppClass);

	void CreateInput();
	void SetWindow(QFAEngineWindow* win);
public:
	QFAEditorMainWindow();
	~QFAEditorMainWindow();
};