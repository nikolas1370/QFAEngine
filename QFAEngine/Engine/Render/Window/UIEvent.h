#pragma once
#include <Tools/Array.h>
#include <Input/Input.h>

class QFAUIUnit;
class QFAViewportRoot;
class QFAWindow;
class QFAUIScroll;
class QFAUIEvent
{
	friend QFAWindow;
	QFAWindow* Window;
public:
	QFAUIEvent();
	~QFAUIEvent();


	

private:
	void Init(QFAWindow* window);
	//in start of array number smaller
	QFAArray<QFAUIUnit*> SortUIUnits;
	void AddUnitToSortList(QFAUIUnit* unit);
	void SortUIs(QFAViewportRoot* root);

	void NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, float delta);
	QFAInput Input;

	float ScrollValue = 0.0f;

	QFAUIUnit* FocusUnit = nullptr;
};



