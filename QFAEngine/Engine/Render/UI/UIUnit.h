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

namespace QFAUISlot
{
	struct SSlotBaseInfo
	{
		QFAUIType::Type typeParent;
		unsigned short structSize;
	};

	// use for parent slot
	struct SParentSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::NONE, sizeof(QFAUISlot::SParentSlot) };
		char ParentSloot[20];
	};

	struct SCanvasSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::Canvas, sizeof(QFAUISlot::SCanvasSlot) };
		float Width = 0.0f;
		float Height = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		bool HeightInPixel = false; // if true Height be in pixel
		bool WidthInPixel = false;
		bool xInPixel = false;
		bool yInPixel = false;
	};

	struct SViewportRootSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::ViewportRoot, sizeof(QFAUISlot::SViewportRootSlot) };
	};

	struct SGridSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::Grid, sizeof(QFAUISlot::SViewportRootSlot) };
	};

	struct SScrollSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::Scroll, sizeof(QFAUISlot::SScrollSlot) };
	};

	struct SListSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::List, sizeof(QFAUISlot::SListSlot) };
		int marginLeft = 0;
		int marginTop = 0;
	};
}

class QFAViewport;
class QFAWindow;
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
class QFAEXPORT QFAUIUnit
{
	friend QFAViewport;
	friend QFAWindow;
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
	QFAUISlot::SCanvasSlot l;
	//IsRoot == true only for QFAViewportRoot
	bool IsRoot = false;

protected:
	bool IsEnable = true;
	bool CanBeParent = false;
	QFAUIType::Type Type = QFAUIType::NONE;
	QFAEditorUIType::Type EditorType = QFAEditorUIType::Type::NONE;

	bool CanRender = false;

	/*
	* if true QFAViewportRoot call SetSizeParent or SetPositionParent
			when QFAViewportRoot resize or move
			need if Unit::CanBeParent = true
	*/
	bool SelfResizable = false;

	int Width = 300;
	int Height = 120;
	int Position_x = 0;
	int Position_y = 0;	
	QFAUIParent* Parent = nullptr;

	float Opacity = 1;
	int ZIndex = 0;
	bool UnitValid = true;
	QFAUISlot::SParentSlot Slot;

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

	// parent set size
	virtual void SetSizeParent(unsigned int w, unsigned int h) = 0;
	// parent set position
	virtual void SetPositionParent(int x, int y) = 0;


	/*
	* call if one of parent was enable or QFAViewport camera enable.
	* and call if parent can be rendered
	*/
	inline virtual void ParentEnable() {}
	// call if one of parent was disable or QFAViewport camera disable.
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
	bool IsValid()
	{
		return this && UnitValid;
	}

	/*
		Position be change if parent is RootUnit
		in other case use Slot
	*/
	virtual void SetPosition(unsigned int x, unsigned int y);
	/*
		Size be change if parent is RootUnit
		in other case use Slot
	*/
	virtual void SetSize(unsigned int w, unsigned int h);

	virtual ~QFAUIUnit();

	inline FVector2D GetPosition()
	{
		return FVector2D((float)Position_x, (float)Position_y);
	}
	FVector2D GetSize()
	{
		return FVector2D((float)Width, (float)Height);
	}
	/*
		all posible slot searsh in namespace QFAUISlot
	*/
	void SetSlot(void* slot);

	bool IsMyParent(QFAUIParent* parent);
	QFAWindow* GetWindow();

	inline QFAUIParent* GetParent()
	{
		return Parent;
	}

	inline void* GetSlot()
	{
		return &Slot;
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
	
	// min max valude is QFAViewport::MinMaxZIndexUI
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