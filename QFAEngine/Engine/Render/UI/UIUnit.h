#pragma once

#include <cstring>
#include <Math/Vector2D.h>
#include <Math/Vector.h>

namespace QFAUIType
{
	enum Type : unsigned char
	{
		NONE = 0,
		Text = 1,
		Canvas = 2,
		ViewportRoot = 3,
		Image = 4,
		Grid = 5
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
		char ParentSloot[16];
	};

	/* 
		all parameter 0 - 1 == 0% - 100%
	*/
	struct SCanvasSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::Canvas, sizeof(QFAUISlot::SCanvasSlot) };
		float Width;
		float Height;
		float x;
		float y;
	};

	struct SViewportRootSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::ViewportRoot, sizeof(QFAUISlot::SViewportRootSlot) };
	};

	struct SGridSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::Grid, sizeof(QFAUISlot::SViewportRootSlot) };
	};
}

class QFAViewport;
class QFAWindow;
class QFAUIParentComponent;
class QFAUICanvas;
class QFAViewportRoot;
class QFAUIGrid;

class QFAUIUnit
{
	friend QFAViewport;
	friend QFAWindow;
	friend QFAUIParentComponent;
	friend QFAUICanvas;
	friend QFAViewportRoot;	
	friend QFAUIGrid;

protected:
	struct UniformOverflow
	{		
		int enable;
		float leftTopX;
		float leftTopY;
		float rightBottomX;
		float rightBottomY;
	};

private:

	QFAUISlot::SCanvasSlot l;




	//IsRoot == true only for QFAViewportRoot
	bool IsRoot = false;
protected:
	bool IsActive = true;

	bool CanBeParent = false;
	QFAUIType::Type Type = QFAUIType::NONE;
	bool CanRender = false;

	/*
	* if true QFAViewportRoot call SetSizeParent or SetPositionParent
			when QFAViewportRoot resize or move
			need if Unit::CanBeParent = true
	*/
	bool SelfResizable = false;

	unsigned int Width = 300;
	unsigned int Height = 120;
	int Position_x = 0;
	// 0 == top 
	int Position_y = 0;	
	QFAUIParentComponent* Parent = nullptr;

	float Opacity = 1;
	float ZIndex = 0; 

	// parent set size
	virtual void SetSizeParent(unsigned int w, unsigned int h) = 0;
	// parent set position
	virtual void SetPositionParent(unsigned int x, unsigned int y) = 0;	
public:
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

	~QFAUIUnit();

	inline FVector2D GetPosition() 
	{
		return FVector2D(Position_x, Position_y);
	}
	FVector2D GetSize()
	{
		return FVector2D(Width, Height);
	}
	/*
		all posible slot searsh in namespace QFAUISlot
	*/
	void SetSlot(void* slot);

	inline QFAUIParentComponent* GetParent()
	{
		return Parent;
	}

	inline bool GetIsRoot()
	{
		return IsRoot;
	}
	

	/*-----*/
	inline void Destroy()
	{
		delete this;
	}


	inline void SetOpacity(float opacity)
	{
		Opacity = opacity;
	}

	// min max valude is QFAViewport::MinMaxZIndexUI
	inline void SetZIndex(float zIndex)
	{
		ZIndex = zIndex * -1;
	}

	inline float  GetZIndex()
	{
		return ZIndex * -1;
	}
protected:
	QFAUISlot::SParentSlot Slot;

	/*
	* call if one of parent was enable or QFAViewport camera enable. 
	* and call if parent can be rendered
	*/
	inline virtual void ParentEnable() {}
	// call if one of parent was disable or QFAViewport camera disable.
	inline virtual void ParentDisable() {}

	/*
	* call if one of parent was attach.
	* call if parent change.
	*/
	inline virtual void ParentAttach() {}
	// call if one of parent was disconect
	inline virtual void ParentDisconect() {}


	void ProcessParentOverflow(UniformOverflow& param, QFAUIParentComponent* parent);
	float ProcessParentOpacity(float childOpacity , QFAUIParentComponent* parent);
};