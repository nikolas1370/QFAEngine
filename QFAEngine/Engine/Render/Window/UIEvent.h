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

	void NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, double delta);
	QFAInput Input;

	float ScrollValue = 0.0f;

	QFAUIUnit* FocusUnit = nullptr;// unit under cursor

	bool LeftMouseDown = false;	
	bool LeftMouseUp = false; 
	bool RightMouseDown = false;
	bool RightMouseUp = false;	
	QFAUIUnit* LeftMouseUnit = nullptr;
	QFAUIUnit* RightMouseUnit = nullptr;


	void FindUnitUnderFocus(QFAViewportRoot* root, QFAUIUnit*& unitUnderFocus, QFAUIScroll*& scrollUnit, float mousePosX, float mousePosY);

	void ScrollEvent(QFAViewportRoot* root, QFAUIScroll* scrollUnit, double delta);
	void FocusEvent(QFAUIUnit* unitUnderFocus);	
	void MouseButtonEvent(QFAUIUnit* newUnitUnderFocus);
};



