#pragma once
#include <Render/UI/Scroll.h>
#include <EditorFileStorage.h>
class QFAUIList;
class QFAEditorGameViewportInfo;
class QActor;
class QFAEditorMainWindow;
class QFAEditorUIActorList : public QFAUIScroll
{
	friend QFAEditorGameViewportInfo;
	friend QFAEditorMainWindow;
	struct SActor
	{
		QActor* actor;
		QFAUIBackground* textBackgroundColor;
	};
	QFAUIList* UIActorList;
	std::vector<SActor> ActorList;

	struct SActorTypes
	{
		size_t fileId;
		size_t count = 0;
	};

	std::vector<SActorTypes> ActorTypes;
public:
	QFAEditorUIActorList();
	~QFAEditorUIActorList();

private:
	void AddActor(QActor* actor, SEditorFile& ef);


	QFAUIBackground* CurentActorInFocus = nullptr;
	QFAUIBackground* CurentActorSelect = nullptr;


	static void Infocus(QFAUIUnit* unit, void* _this);
	static void Outfocus(void* _this);
	static void MouseDown(QFAUIUnit* unit, void* _this);
	void PressDelete();
	/*
		set in QFAEditorMainWindow
		call if some element was select
	*/
	std::function<void()> SelectFun;
	

	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);	
	const QFAColorF OutFocusUnitColor = QFAColorF(0.0f, 0.0f, 0.0f, 0.0f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();

	std::u32string NumToU32string(size_t num);

	void SelectActor(QActor* actor);
};