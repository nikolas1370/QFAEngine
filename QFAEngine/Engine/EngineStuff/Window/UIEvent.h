#pragma once
#include <Tools/Array.h>
#include <Input/Input.h>
#include <vector>

class QFAUIUnit;
class QFAViewportRoot;
class QFAEngineWindow;
class QFAUIScroll;
class QFAUITextInput;
struct GLFWwindow;
class QFAUIEvent
{
	friend QFAEngineWindow;
	friend QFAUIUnit;

	static std::vector<QFAUIEvent*> Events;


	QFAEngineWindow* Window;
	GLFWwindow* glfWindow;
	//in start of array number smaller
	QFAArray<QFAUIUnit*> SortUIUnits;	
	QFAInput* Input;
	float ScrollValue = 0.0f;
	QFAUIUnit* FocusUnit = nullptr;// unit under cursor

	bool LeftMouseDown = false;	
	bool LeftMouseUp = false; 
	bool RightMouseDown = false;
	bool RightMouseUp = false;	

	bool ForwardMouseDown = false;
	bool BackwardMouseDown = false;

	bool LeftShiftDown = false;
	bool RightShiftDown = false;

	QFAUIUnit* LeftMouseUnit = nullptr;
	QFAUIUnit* RightMouseUnit = nullptr;
	QFAUITextInput* TextInput = nullptr;

	static void CharCallback(GLFWwindow* window, unsigned int codepoint);
	// call if QFAUIUnit delete
	static void UnitUnderDelete(QFAUIUnit* deadUnit);
	// if _glfWindow == null this set in not regular window
	QFAUIEvent(QFAEngineWindow* window, GLFWwindow* _glfWindow);
	~QFAUIEvent();
	void AddUnitToSortList(QFAUIUnit* unit);
	void SortUIs(QFAViewportRoot* root);

	void NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, double delta, bool regularWindow);

	void FindUnitUnderFocus(QFAViewportRoot* root, QFAUIUnit*& unitUnderFocus, QFAUIScroll*& scrollUnit, float mousePosX, float mousePosY, bool regularWindow);

	void ScrollEvent(QFAViewportRoot* root, QFAUIScroll* scrollUnit, double delta);
	void FocusEvent(QFAUIUnit* unitUnderFocus);	
	void MouseButtonEvent(QFAUIUnit* newUnitUnderFocus);
	void InputFocusEvent(QFAUIUnit* newUnitUnderFocus);
};