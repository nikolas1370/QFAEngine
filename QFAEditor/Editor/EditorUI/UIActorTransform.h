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

	static QFAUIActorTransform* ActorTransform;
	QFAUICanvas* Canvas;
	/*
		[0, 1, 2] Position
		[3, 4, 5] Rotation
		[6, 7, 8] Scale	
	*/
	std::array<QFAUITextInput*, 9> Inputs;
	std::array<QFAUIBackground*, 9> InputBackgrounds;
	
public:
	QFAUIActorTransform();
	~QFAUIActorTransform();

private:
	void MySlotChange(QFAUIUnit* unit) override;
	void ChangeSize(unsigned int w, unsigned int h) override;
	
	void ChangePosition(int x, int y) override;


	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;

	const int InputHeight = 25;

	static void InputOut(QFAUITextInput* input);

	QActor* CurentSelectActor = nullptr;
	void SelectActor(QActor* actor);
};
