#pragma once
#include <Tools/Array.h>
#include <Input/Input.h>

class QFAUIUnit;
class QFAViewportRoot;
class QFAWindow;
class QFAUIScroll;
class QFAUITextInput;
class QFAUIEvent
{
	friend QFAWindow;
	QFAWindow* Window;
	struct SEvent
	{
		QFAUIEvent* event;
		GLFWwindow* glfWindow;
	};

	static SEvent MainEvent;
public:
	QFAUIEvent();
	~QFAUIEvent();


	

private:
	void Init(QFAWindow* window, GLFWwindow* _glfWindow);
	GLFWwindow* glfWindow;
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
	QFAUITextInput* TextInput = nullptr;

	void FindUnitUnderFocus(QFAViewportRoot* root, QFAUIUnit*& unitUnderFocus, QFAUIScroll*& scrollUnit, float mousePosX, float mousePosY);

	void ScrollEvent(QFAViewportRoot* root, QFAUIScroll* scrollUnit, double delta);
	void FocusEvent(QFAUIUnit* unitUnderFocus);	
	void MouseButtonEvent(QFAUIUnit* newUnitUnderFocus);
	void InputFocusEvent(QFAUIUnit* newUnitUnderFocus);


	static void CharCallback(GLFWwindow* window, unsigned int codepoint);
};



