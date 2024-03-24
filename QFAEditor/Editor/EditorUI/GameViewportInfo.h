#pragma once
#include <Render/UI/UIParentHiddenChild.h>

class QFAUIScroll;
class QFAUIList;
class QActor;
class QFAUICanvas;
class QFAEditorUIActorList;
class QFAEditorMainWindow;

class QFAEditorGameViewportInfo : public QFAParentHiddenChild
{
	friend QFAEditorMainWindow;

	QFAEditorUIActorList* ActorList;



	void MySlotChange(QFAUIUnit* unit) override;


	void ChangeSize(unsigned int w, unsigned int h) override;

	void ChangePosition(int x, int y) override;

	float UpdateInnerHeight() override;
	float UpdateInnerWidth() override;
public:
	QFAEditorGameViewportInfo();
	~QFAEditorGameViewportInfo();

	
private:



};
