#pragma once
#include <Render/UI/UIParentHiddenChild.h>
#include <EditorFileStorage.h>
class QFAUIScroll;
class QFAUIList;
class QActor;
class QFAUICanvas;
class QFAEditorUIActorList;
class QFAEditorMainWindow;
class QFAUIActorTransform;
class QFAUIGrid;
class QFAUISelectList;
class QFATextBackground;

class QFAEditorGameViewportInfo : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;
	friend QFAEditorUIActorList;
	struct SActor
	{
		QActor* actor;
		QFATextBackground* text;
	};

	struct SActorTypes
	{
		size_t fileId;
		size_t count = 0;
	};

	QFAUICanvas* Canvas;
	QFAUIList* ActorInfoList;// transforma and etc

	QFAUISelectList* ActorList;
	std::vector<SActor> ActorAndTextList;
	std::vector<SActorTypes> ActorTypes;

	QFAUIActorTransform* ActorTransform;

	void MySlotChange(QFAUIUnit* unit) override;
	void ChangeSize(unsigned int w, unsigned int h) override;

	void ChangePosition(int x, int y) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
public:
	QFAEditorGameViewportInfo();
	~QFAEditorGameViewportInfo();

	
private:
	void SelectActor(QActor* actor);
	void AddActor(QActor* actor, SEditorFile& ef);
	void PressedDelete();

	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();
	const QFAColorF SelectUnitNotFocus = QFAColor(64, 87, 111, 255).GetColorF();
};
