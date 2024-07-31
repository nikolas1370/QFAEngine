#pragma once
#include <UI/UIParentHiddenChild.h>
class QFAUICanvas;
class QFAUITextInput;
class QFAUIBackground;
class QActor;
class QFAEditorGameViewportInfo;

class QFAUIActorTransform : public QFAParentHiddenChild
{
	friend QFAEditorGameViewportInfo;

	const int InputHeight = 25;	
	const char* InputHeightChar = "25";

	static QFAUIActorTransform* ActorTransform;

	QActor* CurentSelectActor = nullptr;
	QFAUICanvas* Canvas;
	/*
		[0, 1, 2] Position
		[3, 4, 5] Rotation
		[6, 7, 8] Scale	
	*/
	std::array<QFAUITextInput*, 9> Inputs;
	
	static void InputOut(QFAUITextInput* input);

	void MySlotChange(QFAUIUnit* unit) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
	void SelectActor(QActor* actor);

protected:
	void WidthChanged(int oldValue = 0) override;
	void HeightChanged(int oldValue = 0) override;
	void TopChanged(int oldValue = 0) override;
	void LeftChanged(int oldValue = 0) override;

public:
	QFAUIActorTransform();
	~QFAUIActorTransform();
};
