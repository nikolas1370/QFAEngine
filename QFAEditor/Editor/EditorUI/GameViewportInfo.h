#pragma once
#include <UI/UIParentHiddenChild.h>
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
		bool isCpp;
	};

	const QFAColorF InFocusUnitColor = QFAColorF(1.0f, 1.0f, 1.0f, 0.05f);
	const QFAColorF SelectUnit = QFAColor(14, 134, 255, 255).GetColorF();
	const QFAColorF SelectUnitNotFocus = QFAColor(64, 87, 111, 255).GetColorF();

	QFAUICanvas* Canvas;
	QFAUIList* ActorInfoList;// transforma and etc

	QFAUISelectList* ActorList;
	std::vector<SActor> ActorAndTextList;
	std::vector<SActorTypes> ActorTypes;

	QFAUIActorTransform* ActorTransform;

	void MySlotChange(QFAUIUnit* unit) override;


	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	void SelectActor(QActor* actor);
	void AddActor(QActor* actor, std::u32string actorName, size_t id, bool isCppClass);
	// return index in actor list
	QActor* PressedDelete();


protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAEditorGameViewportInfo();
	~QFAEditorGameViewportInfo();
};
