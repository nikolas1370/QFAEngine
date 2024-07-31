#pragma once

#include <cstring>
#include <Math/Vector2D.h>
#include <Math/Vector.h>
#include <functional>
#include <Tools/Stuff.h>
namespace QFAUIType
{
	enum Type : unsigned char
	{
		NONE = 0,
		Text = 1,
		Canvas = 2,
		ViewportRoot = 3,
		Image = 4,
		Grid = 5,
		Scroll = 6,
		TextInput = 7,
		List = 8,
		CustomUnit = 9,
		Background = 10
	};
}

namespace QFAEditorUIType
{
	enum Type : unsigned char
	{
		NONE = 0,
		FileExplorer = 1,
		ExplorerFolderUnit = 2,
		FileViewUnit = 3,
	};
}

class QFAEngineViewport;
class QFAEngineWindow;
class QFAUIParentMultipleUnit;
class QFAUICanvas;
class QFAViewportRoot;
class QFAUIGrid;
class QFAUIEvent;
class QFAUIParentOneUnit;
class QFAUIParent;
class QFAUIScroll;
class QFAViewportRoot;
class QFAUIList;
class QFAParentHiddenChild;
class QFAUIBackground;
class QFAUISelectUnit;
class QFAEXPORT QFAUIUnit
{
	friend QFAEngineViewport;
	friend QFAEngineWindow;
	friend QFAUIParentMultipleUnit;
	friend QFAUICanvas;
	friend QFAViewportRoot;	
	friend QFAUIGrid;
	friend QFAUIEvent;
	friend QFAUIParentOneUnit;
	friend QFAUIScroll;
	friend QFAUIList;
	friend QFAParentHiddenChild;
	friend QFAUIParent;
	friend QFAUIBackground;
	friend QFAUISelectUnit;

protected:
	struct UniformOverflow
	{		
		int enable;
		float leftTopX;
		float leftTopY;
		float rightBottomX;
		float rightBottomY;
	};
public:
	class QFAEXPORT EventFunctions
	{
		struct Callback
		{
			void (*fun)(void* userData) = nullptr;
			void* userData = nullptr;
		};

		struct CallbackWithUnit
		{
			void (*fun)(QFAUIUnit* unitInFocus, void* userData) = nullptr;
			void* userData = nullptr;
		};// 

		friend QFAUIUnit;
		CallbackWithUnit InFocus;
		Callback OutFocus;

		CallbackWithUnit LeftMouseDown;
		CallbackWithUnit LeftMouseUp;
		CallbackWithUnit RightMouseDown;
		CallbackWithUnit RightMouseUp;

		CallbackWithUnit LeftMouseDownUp;
		CallbackWithUnit RightMouseDownUp;

		CallbackWithUnit ForwardMouseDown;
		CallbackWithUnit BackwardMouseDown;

		EventFunctions() {};
	public:
		/*
			QFAUIUnit* unit,
			void* userData
		*/

		/*
			if child have focus parents also have focus
			QFAUIUnit* unit in focus
		*/
		void SetInFocus(void (*fun)(QFAUIUnit*, void*), void* userData);
		/*
			if child have outfocus parents
			also have outfocus only if parent
			not have new child in focus or self in focus.
			if parents have new child focus
			parents get event InFocus with new focus unit.
		*/
		void SetOutFocus(void (*fun)(void*), void* userData);

		/*
			parents also notified
		*/
		void SetLeftMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData);
		void SetLeftMouseUp(void (*fun)(QFAUIUnit*, void*), void* userData);
		void SetRightMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData);
		void SetRightMouseUp(void (*fun)(QFAUIUnit*, void*), void* userData);
		void SetForwardMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData);
		void SetBackwardMouseDown(void (*fun)(QFAUIUnit*, void*), void* userData);

		/*
			call if Left Mouse button press and release at same unit
			parents also notified
		*/
		void SetLeftMouseDownUp(void (*fun)(QFAUIUnit*, void*), void* userData);
		/*
			call if Right Mouse button press and release at same unit
			parents also notified
		*/
		void SetRightMouseDownUp(void (*fun)(QFAUIUnit*, void*), void* userData);
	};

private:
	//IsRoot == true only for QFAViewportRoot
	bool IsRoot = false;

protected:
	const char* StrTop = nullptr;
	const char* StrLeft = nullptr;
	const char* StrWidth = nullptr;
	const char* StrHeight = nullptr;

	bool IsEnable = true;
	bool CanBeParent = false; // set it if unit parent class
	QFAUIType::Type Type = QFAUIType::NONE;
	QFAEditorUIType::Type EditorType = QFAEditorUIType::Type::NONE;

	bool CanRender = false;

	int Width = 300;
	int Height = 120;
	int Position_x = 0;
	int Position_y = 0;	

	/*
		parent want child have this size or position
		but child can ignot it if user say other paremeter
	*/	
	int ParentSetWidth = 0, ParentSetHeight = 0, 
		ParentSetPosition_x = 0, ParentSetPosition_y = 0;
	

	QFAUIParent* Parent = nullptr;

	float Opacity = 1;
	int ZIndex = 0;
	bool UnitValid = true;
	bool ParentSetWidthMinus, ParentSetHeightMinus; // set in SetWidth SetHeight

	/*
		need for Scroll
		Height inside unit
		if innerHeight < Height, Scroll use Height
	*/
	unsigned int InnerHeight = 0;
	unsigned int InnerWidth = 0;

public:
	std::string UnitName;
	EventFunctions Events;

protected:

	// *Changed() use for change size or position in child (childUnit->SetWidth) after use set*() in parent unit
	// call in SetWidth
	virtual void WidthChanged(int oldValue = 0) = 0;
	// call in SetHeight
	virtual void HeightChanged(int oldValue = 0) = 0;
	// call in SetTop
	virtual void TopChanged(int oldValue = 0) = 0;
	// call in SetLeft
	virtual void LeftChanged(int oldValue = 0) = 0;

	/*
	* call if one of parent was enable or QFAEngineViewport camera enable.
	* and call if parent can be rendered
	*/
	inline virtual void ParentEnable() {}
	// call if one of parent was disable or QFAEngineViewport camera disable.
	inline virtual void ParentDisable() {}

	/*
	* call if this unit was disconect or one of parent was attach.
	* call if parent change.
	*/
	inline virtual void ParentAttach() {}
	/*
		call if this unit was disconect or one of parent was disconect
		if Parent == null this unit be disconect from parent
	*/
	inline virtual void ParentDisconect() {}


	void ProcessParentOverflow(UniformOverflow& param, QFAUIParent* parent);
	float ProcessParentOpacity(float childOpacity, QFAUIParent* parent);




protected:
	/*
						Notify this unit and all parents of infocus event
							call FunInFocus
					*/
	void NotifyInFocus();
	/*
		Notify this unit and all parents of outfocus event
			call FunOutFocus

		if onlyOneUnit == true Notify only this unit
	*/
	void NotifyOutFocus(bool onlyOneUnit);


	void NotifyLeftMouseDown();
	void NotifyLeftMouseUp();
	void NotifyRightMouseDown();
	void NotifyRightMouseUp();

	void NotifyForwardMouseDown();
	void NotifyBackwardMouseDown();

	void NotifyLeftMouseDownUp();
	void NotifyRightMouseDownUp();

public:
	/* width == "10 + 7%" or null
	if parentSetWidthMinus == true , parentSetWidth - width::value
	if parentSetWidthMinus == false, Width be width::value
*/
	virtual void SetWidth(const char* width, bool parentSetWidthMinus = false) final;
	/*
		if parentSetHeightMinus == true , parentSetHeight - height::value
		if parentSetHeightMinus == false, Height be height::value
	*/
	virtual void SetHeight(const char* height, bool parentSetHeightMinus = false) final;

	// set offset from ParentSetPosition_y
	virtual void SetTop(const char* top) final;
	// set offset from ParentSetPosition_x
	virtual void SetLeft(const char* left) final;

	bool IsValid()
	{
		return this && UnitValid;
	}

	virtual ~QFAUIUnit();

	inline FVector2D GetPosition()
	{
		return FVector2D((float)Position_x, (float)Position_y);
	}
	FVector2D GetSize()
	{
		return FVector2D((float)Width, (float)Height);
	}

	bool IsMyParent(QFAUIParent* parent);
	QFAEngineWindow* GetWindow();

	inline QFAUIParent* GetParent()
	{
		return Parent;
	}

	inline bool GetIsRoot()
	{
		return IsRoot;
	}
	
	inline bool GetEnable()
	{
		return IsEnable;
	}

	inline void SetEnable(bool enable)
	{
		IsEnable = enable;
	}

	inline void Destroy()
	{
		if (IsValid())
		{
			UnitValid = false;
			delete this;
		}
	}

	inline void SetOpacity(float opacity)
	{
		Opacity = opacity;
	}
	
	// min max valude is QFAEngineViewport::MinMaxZIndexUI
	inline void SetZIndex(int zIndex)
	{
		ZIndex = zIndex;
	}

	inline int GetZIndex()
	{
		return ZIndex ;
	}

	/*
		return null if viewport Root not found

		countUnit == amount parentsUnit of this unit plus one
	*/
	QFAViewportRoot* GetViewportRoot(unsigned int& countUnit);

	inline QFAUIType::Type GetUnitType()
	{
		return Type;
	}
	// rename GetEditoUnitType to GetEditorUnitType
	inline QFAEditorUIType::Type GetEditoUnitType()
	{
		return EditorType;
	}
};


/*

QFAUIImage don


коли парент ставить дить мінять розмір його

*/