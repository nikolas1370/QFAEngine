#pragma once
#include <functional>
#include <Tools/Array.h>
#include <string>
#include <Math/Vector2D.h>
#include <Math/Vector.h>
#include <Tools/Stuff.h>

namespace EKey
{
	enum Key : int
	{// add ANY =,
		UNKNOWN			= -1,	//GLFW_KEY_UNKNOWN
		/*-- mouse */
		MOUSE_LEFT		= 0,	//GLFW_MOUSE_BUTTON_1
		MOUSE_RIGHT		= 1,	//GLFW_MOUSE_BUTTON_2
		MOUSE_MIDDLE	= 2,	//GLFW_MOUSE_BUTTON_3
		MOUSE_4			= 3,	//GLFW_MOUSE_BUTTON_4
		MOUSE_5			= 4,	//GLFW_MOUSE_BUTTON_5
		MOUSE_6			= 5,	//GLFW_MOUSE_BUTTON_6
		MOUSE_7			= 6,	//GLFW_MOUSE_BUTTON_7
		MOUSE_8			= 7,	//GLFW_MOUSE_BUTTON_8
		/* -- keyboard  - */
		SPACE			= 32,	//GLFW_KEY_SPACE
		APOSTROPHE		= 39,	//GLFW_KEY_APOSTROPHE   /* ' */
		COMMA			= 44,	//GLFW_KEY_COMMA		/* , */
		MINUS			= 45,	//GLFW_KEY_MINUS		/* - */
		PERIOD			= 46,	//GLFW_KEY_PERIOD		/* . */
		SLASH			= 47,	//GLFW_KEY_SLASH		/* / */
		ZERO			= 48,	//GLFW_KEY_0
		ONE				= 49,	//GLFW_KEY_1
		TWO				= 50,	//GLFW_KEY_2
		THREE			= 51,	//GLFW_KEY_3
		FOUR			= 52,	//GLFW_KEY_4
		FIVE			= 53,	//GLFW_KEY_5
		SIXE			= 54,	//GLFW_KEY_6
		SEVEN			= 55,	//GLFW_KEY_7
		EIGHT			= 56,	//GLFW_KEY_8
		NINE			= 57,	//GLFW_KEY_9
		SEMICOLON		= 59,	//GLFW_KEY_SEMICOLON	/* ; */
		EQUAL			= 61,	//GLFW_KEY_EQUAL			/* = */
		A				= 65,	//GLFW_KEY_A
		B				= 66,	//GLFW_KEY_B
		C				= 67,	//GLFW_KEY_C
		D				= 68,	//GLFW_KEY_D
		E				= 69,	//GLFW_KEY_E
		F				= 70,	//GLFW_KEY_F
		G				= 71,	//GLFW_KEY_G
		H				= 72,	//GLFW_KEY_H
		I				= 73,	//GLFW_KEY_I
		J				= 74,	//GLFW_KEY_J
		K				= 75,	//GLFW_KEY_K
		L				= 76,	//GLFW_KEY_L
		M				= 77,	//GLFW_KEY_M
		N				= 78,	//GLFW_KEY_N
		O				= 79,	//GLFW_KEY_O
		P				= 80,	//GLFW_KEY_P
		Q				= 81,	//GLFW_KEY_Q
		R				= 82,	//GLFW_KEY_R
		S				= 83,	//GLFW_KEY_S
		T				= 84,	//GLFW_KEY_T
		U				= 85,	//GLFW_KEY_U
		V				= 86,	//GLFW_KEY_V
		W				= 87,	//GLFW_KEY_W
		X				= 88,	//GLFW_KEY_X
		Y				= 89,	//GLFW_KEY_Y
		Z				= 90,	//GLFW_KEY_Z
		LEFT_BRACKET	= 91,   //GLFW_KEY_LEFT_BRACKET,	/* [ */
		BACKSLASH		= 92,	//GLFW_KEY_BACKSLASH,		/* \ */
		RIGHT_BRACKET	= 93,	//GLFW_KEY_RIGHT_BRACKET,	/* ] */
		GRAVE_ACCENT	= 96,	//GLFW_KEY_GRAVE_ACCENT	/* ` */
		/* Function keys */
		ESCAPE			= 256,	//GLFW_KEY_ESCAPE,
		ENTER			= 257,	//GLFW_KEY_ENTER,
		TAB				= 258,	//GLFW_KEY_TAB,
		BACKSPACE		= 259,	//GLFW_KEY_BACKSPACE,
		INSERT			= 260,	//GLFW_KEY_INSERT,
		DELETE_KEY		= 261,	//GLFW_KEY_DELETE,
		RIGHT			= 262,	//GLFW_KEY_RIGHT,
		LEFT			= 263,	//GLFW_KEY_LEFT,
		DOWN			= 264,	//GLFW_KEY_DOWN,
		UP				= 265,	//GLFW_KEY_UP,
		PAGE_UP			= 266,	//GLFW_KEY_PAGE_UP,
		PAGE_DOWN		= 257,	//GLFW_KEY_PAGE_DOWN,
		HOME			= 258,	//GLFW_KEY_HOME,
		END				= 269,	//GLFW_KEY_END,
		CAPS_LOCK		= 280,	//GLFW_KEY_CAPS_LOCK,
		SCROLL_LOCK		= 281,	//GLFW_KEY_SCROLL_LOCK,
		NUM_LOCK		= 282,	//GLFW_KEY_NUM_LOCK,
		PRINT_SCREEN	= 283,	//GLFW_KEY_PRINT_SCREEN,
		PAUSE			= 284,	//GLFW_KEY_PAUSE,
		F1				= 290,	//GLFW_KEY_F1,
		F2				= 291,	//GLFW_KEY_F2,
		F3				= 292,	//GLFW_KEY_F3,
		F4				= 293,	//GLFW_KEY_F4,
		F5				= 294,	//GLFW_KEY_F5,
		F6				= 295,	//GLFW_KEY_F6,
		F7				= 296,	//GLFW_KEY_F7,
		F8				= 297,	//GLFW_KEY_F8,
		F9				= 298,	//GLFW_KEY_F9,
		F10				= 299,	//GLFW_KEY_F10,
		F11				= 300,	//GLFW_KEY_F11,
		F12				= 301,	//GLFW_KEY_F12,
		F13				= 302,	//GLFW_KEY_F13,
		F14				= 303,	//GLFW_KEY_F14,
		F15				= 304,	//GLFW_KEY_F15,
		F16				= 305,	//GLFW_KEY_F16,
		F17				= 306,	//GLFW_KEY_F17,
		F18				= 307,	//GLFW_KEY_F18,
		F19				= 308,	//GLFW_KEY_F19,
		F20				= 309,	//GLFW_KEY_F20,
		F21				= 310,	//GLFW_KEY_F21,
		F22				= 311,	//GLFW_KEY_F22,
		F23				= 312,	//GLFW_KEY_F23,
		F24				= 313,	//GLFW_KEY_F24,
		F25				= 314,	//GLFW_KEY_F25,		
		NUMPAD_0		= 320,	//GLFW_KEY_KP_0,
		NUMPAD_1		= 321,	//GLFW_KEY_KP_1,
		NUMPAD_2		= 322,	//GLFW_KEY_KP_2,
		NUMPAD_3		= 323,	//GLFW_KEY_KP_3,
		NUMPAD_4		= 324,	//GLFW_KEY_KP_4,
		NUMPAD_5		= 325,	//GLFW_KEY_KP_5,
		NUMPAD_6		= 326,	//GLFW_KEY_KP_6,
		NUMPAD_7		= 327,	//GLFW_KEY_KP_7,
		NUMPAD_8		= 328,	//GLFW_KEY_KP_8,
		NUMPAD_9		= 329,	//GLFW_KEY_KP_9,
		NUMPAD_DECIMAL  = 330,	//GLFW_KEY_KP_DECIMAL,
		NUMPAD_DIVIDE   = 331,	//GLFW_KEY_KP_DIVIDE,
		NUMPAD_MULTIPLY = 332,	//GLFW_KEY_KP_MULTIPLY,
		NUMPAD_SUBTRACT = 333,	//GLFW_KEY_KP_SUBTRACT,
		NUMPAD_ADD		= 334,	//GLFW_KEY_KP_ADD,
		NUMPAD_ENTER    = 335,	//GLFW_KEY_KP_ENTER,
		NUMPAD_EQUAL    = 336,	//GLFW_KEY_KP_EQUAL,		
		LEFT_SHIFT		= 340,	//GLFW_KEY_LEFT_SHIFT,
		LEFT_CONTROL	= 341,	//GLFW_KEY_LEFT_CONTROL,
		LEFT_ALT		= 342,	//GLFW_KEY_LEFT_ALT,
		LEFT_SUPER		= 343,	//GLFW_KEY_LEFT_SUPER,
		LEFT_WIN		= 343,	//GLFW_KEY_LEFT_SUPER,
		RIGHT_SHIFT		= 344,	//GLFW_KEY_RIGHT_SHIFT,
		RIGHT_CONTROL	= 345,	//GLFW_KEY_RIGHT_CONTROL,
		RIGHT_ALT		= 346,	//GLFW_KEY_RIGHT_ALT,
		RIGHT_SUPER		= 347,	//GLFW_KEY_RIGHT_SUPER,
		RIGHT_WIN		= 347,	//GLFW_KEY_RIGHT_SUPER,
		MENU			= 348,	//GLFW_KEY_MENU
	};
}

class QFAOverlord;
struct GLFWwindow;
class QFAInputAxis1D;
class QFAInputAxis2D;
class QFAInputAxis3D;
class QFAEngineWindow;
/*
	Event process before Tick
*/
class QFAEXPORT QFAInput
{
	friend QFAEngineWindow; 
	friend QFAOverlord;
	friend QFAInputAxis1D;
	friend QFAInputAxis2D;
	friend QFAInputAxis3D;

	struct SKeyFunction
	{		
		std::function<void(EKey::Key)> Fun;
		EKey::Key Key;
		std::string Id;
	};

	struct SPressedAnyKey
	{
		std::function<void(EKey::Key)> Fun;
		bool Active = false;
	};

	struct SWheelAxis
	{
		std::function<void(float)> Fun;
		bool Active = false;
	};

	struct SMouseMove
	{
		std::function<void(FVector2D)> Fun;
		bool Active = false;
	};
	
	struct SKeyHold
	{
		std::function<void(EKey::Key)> Fun;
		EKey::Key Key;
		std::string Id;
		float HoldTime;
		float TimeButtonPressed = 0.0f;
		bool Pressed = false;
	};

	struct SKeyAxis1D
	{		
		EKey::Key Key;
		std::string Id;
		float AxisValue;
		bool Pressed = false;				
	};

	struct SAxis1D
	{	
		std::string Id;
		std::function<void(float)> Fun;
		QFAArray<SKeyAxis1D> Keys;
	};

	struct SKeyAxis2D
	{
		EKey::Key Key;
		std::string Id;
		FVector2D AxisValue;
		bool Pressed = false;
	};

	struct SAxis2D
	{
		std::string Id;
		std::function<void(FVector2D)> Fun;
		QFAArray<SKeyAxis2D> Keys;
	};

	struct SKeyAxis3D
	{
		EKey::Key Key;

		std::string Id;
		FVector AxisValue;
		bool Pressed = false;
	};

	struct SAxis3D
	{
		std::string Id;
		std::function<void(FVector)> Fun;
		QFAArray<SKeyAxis3D> Keys;
	};

	struct Sinput
	{
		QFAEngineWindow* Window;
		QFAInput* Input;
		QFAInput* operator->() const
		{
			return Input;
		}
	};

	static std::vector<QFAEngineWindow*> WindowList;
	static QFAArray<Sinput> Inputs;
	static FVector2D LastMousePosition;

	GLFWwindow* Window;
	GLFWwindow* GlfWindow;
	
	bool InputValid = true;

	QFAArray<SKeyFunction> KeyPressList;
	QFAArray<SKeyFunction> KeyReleaseList;
	QFAArray<SKeyHold> KeyHoldList;
	QFAArray<SAxis1D> AxisList;
	QFAArray<SAxis2D> Axis2DList;
	QFAArray<SAxis3D> Axis3DList;
	bool BlockInput = false;

	SPressedAnyKey Any;
	SWheelAxis WheelAxis;
	SMouseMove MouseMove;
	SMouseMove MouseMoveAxis;	

	/*
		GLFW_REPEAT not match with count frame in game
		call after glfwPollEvents
	*/
	static void NewFrame(float delta);
	/*
	 *  @param[in] window The window that received the event.
	 *  @param[in] key The [keyboard key](@ref keys) that was pressed or released.
	 *  @param[in] scancode The system-specific scancode of the key.
	 *  @param[in] action `GLFW_PRESS`, `GLFW_RELEASE` or `GLFW_REPEAT`.  Future
	 *		releases may add more actions.
	 *  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
	 *  held down.
	*/
	static void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void MouseMove_callback(GLFWwindow* window, float xoffset, float yoffset);
	static void ProcessKey(GLFWwindow* _window, int key, int scancode, int action, int mods);
	/*
		call when QFAEngineWindow was created
	*/
	static void WindowCreated(QFAEngineWindow* window);
	/*
		call when QFAEngineWindow was closed
	*/
	static void WindowClosed(QFAEngineWindow* window);
	
	/*
	fun be call every frame even when button not push
*/
	static void AddKeyToAxis1D(QFAInput* input, std::string axisId, EKey::Key key, float axisValue, std::string keyId);
	static void RemoveKeyFromAxis1D(QFAInput* input, std::string axisId, std::string keyId);

	/*
		fun be call every frame even when button not push
	*/
	static void AddKeyToAxis2D(QFAInput* input, std::string axisId, EKey::Key key, FVector2D axisValue, std::string keyId);
	static void RemoveKeyFromAxis2D(QFAInput* input, std::string axisId, std::string keyId);

	/*
		fun be call every frame even when button not push
	*/
	static void AddKeyToAxis3D(QFAInput* input, std::string axisId, EKey::Key key, FVector axisValue, std::string keyId);
	static void RemoveKeyFromAxis3D(QFAInput* input, std::string axisId, std::string keyId);

public:
	/*
		input attach to main window
	*/
	QFAInput();
	/*
		input attach to *window
	*/
	QFAInput(QFAEngineWindow* window);
	~QFAInput();

	void ChangeWindow(QFAEngineWindow* window);

	void ActiveInput(bool activate)
	{
		BlockInput = !activate;
	}
	/*
		if id exis Ekey::key be changed
	*/
	void AddKeyPress(EKey::Key key, std::string id, std::function<void(EKey::Key)> fun);
	void RemoveKeyPress(std::string Id);

	/*
		if id exis Ekey::key be changed
	*/
	void AddKeyRelease(EKey::Key key, std::string id, std::function<void(EKey::Key)> fun);
	void RemoveKeyRelease(std::string Id);

	/*
		fun call one time
	*/
	void SetPressedAnyKey(std::function<void(EKey::Key)> fun);
	/*
		if PressedAnyKey fun set call one time
	*/
	void ActivePressedAnyKey();
	void ShutOffPressedAnyKey();

	void SetWheelAxis(std::function<void(float)> fun);
	void ShutOffWheelAxis();

	void SetMouseMove(std::function<void(FVector2D)> fun);
	void ShutOffMouseMove();

	/* FVector2D content offset after last move*/
	void SetMouseMoveAxis(std::function<void(FVector2D)> fun);
	void ShutOffMouseMoveAxis();

	/*
	if id exis Ekey::key be changed
	*/
	void AddKeyHold(EKey::Key key, std::string id, float holdTime, std::function<void(EKey::Key)> fun);
	void RemoveKeyHold(std::string Id);

	/*
	if id exis return last created QFAInputAxis1D
	fun call every frame
	*/
	QFAInputAxis1D CreateAxis1D(std::string id, std::function<void(float)> fun);
	void RemoveAxis1D(std::string id);

	/*
	if id exis return last created QFAInputAxis2D
	fun call every frame
	*/
	QFAInputAxis2D CreateAxis2D(std::string id, std::function<void(FVector2D)> fun);
	void RemoveAxis2D(std::string id);

	/*
	if id exis return last created QFAInputAxis3D
	fun call every frame
	*/
	QFAInputAxis3D CreateAxis3D(std::string id, std::function<void(FVector)> fun);
	void RemoveAxis3D(std::string id);


	inline bool IsValide()
	{
		return this && InputValid;
	}
};


class QFAInputAxis1D 
{
	friend QFAInput;
	std::string Id;
	QFAInput* Input;
public:
	QFAInputAxis1D() {};
	QFAInputAxis1D(QFAInput* input, std::string id)
	{
		Id = id;
		Input = input;
	}

	// if id exis Ekey::key be changed
	inline void AddKey(EKey::Key key, float axisValue, std::string id)
	{
		QFAInput::AddKeyToAxis1D(Input ,Id, key, axisValue, id);
	}

	inline void RemoveKey(std::string id)
	{
		QFAInput::RemoveKeyFromAxis1D(Input, Id, id);
	}
};

class QFAInputAxis2D
{
	friend QFAInput;
	std::string Id;
	QFAInput* Input;
public:
	QFAInputAxis2D() {};
	QFAInputAxis2D(QFAInput* input, std::string id)
	{
		Id = id;
		Input = input;
	}

	// if id exis Ekey::key be changed
	inline void AddKey(EKey::Key key, FVector2D axisValue, std::string id)
	{
		QFAInput::AddKeyToAxis2D(Input,Id, key, axisValue, id);
	}

	inline void RemoveKey(std::string id)
	{
		QFAInput::RemoveKeyFromAxis2D(Input, Id, id);
	}
};

class QFAInputAxis3D
{
	friend QFAInput;
	std::string Id;
	QFAInput* Input;
public:
	QFAInputAxis3D() {};
	QFAInputAxis3D(QFAInput* input, std::string id)
	{
		Id = id;
		Input = input;
	}
	// if id exis Ekey::key be changed
	inline void AddKey(EKey::Key key, FVector axisValue, std::string id)
	{		
		QFAInput::AddKeyToAxis3D(Input,  Id, key, axisValue, id);
	}

	inline void RemoveKey(std::string id)
	{
		QFAInput::RemoveKeyFromAxis3D(Input, Id, id);
	}
};
