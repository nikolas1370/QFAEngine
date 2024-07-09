#pragma once
#include <Render/UI/UIParentHiddenChild.h>
class QFAUICanvas;
class QFAUITextInput;
class QFAUIBackground;
class QActor;
class QFAEditorGameViewportInfo;

class QFAUIActorTransform : public QFAParentHiddenChild
{
	friend QFAEditorGameViewportInfo;

	const int InputHeight = 25;	

	static QFAUIActorTransform* ActorTransform;

	QActor* CurentSelectActor = nullptr;
	QFAUICanvas* Canvas;
	/*
		[0, 1, 2] Position
		[3, 4, 5] Rotation
		[6, 7, 8] Scale	
	*/
	std::array<QFAUITextInput*, 9> Inputs;
	std::array<QFAUIBackground*, 9> InputBackgrounds;
	
	static void InputOut(QFAUITextInput* input);

	void MySlotChange(QFAUIUnit* unit) override;
	void ChangeSize(unsigned int w, unsigned int h) override;

	void ChangePosition(int x, int y) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	void SelectActor(QActor* actor);

public:
	QFAUIActorTransform();
	~QFAUIActorTransform();
};
