#pragma once

#include <cstring>
#include <Math/Vector2D.h>

namespace QFAUIType
{
	enum Type : unsigned char
	{
		NONE = 0,
		Text = 1,
		Canvas = 2
	};
}

namespace QFAUISlot
{
	// use for parent slot
	struct SParentSlot
	{
		QFAUIType::Type typeParent;
		unsigned short structSize = sizeof(SParentSlot);
		float ParentSloot[4];
	};

	struct SCanvasSlot
	{/* 0 - 1 */
		QFAUIType::Type typeParent = QFAUIType::Canvas;
		unsigned short structSize = sizeof(SCanvasSlot);
		float Width;
		float Height;
		float x;
		float y;
	};
}

class QFAViewport;
class QFAWindow;
class QFAUIParentComponent;
class QFAUICanvas;
class QFAUIUnit
{
	friend QFAViewport;
	friend QFAWindow;
	friend QFAUIParentComponent;
	friend QFAUICanvas;
	// call if viewport change own size
	virtual void ViewportSizeChange(int viewportWidth, int viewportHeight)
	{

	}
protected:


	bool CanBeParent = false;
	QFAUIType::Type Type = QFAUIType::NONE;
	bool CanRender = false;

	// if QFAViewport can call ViewportSizeChange
	bool SelfResizable = false;

	unsigned int Width = 300;
	unsigned int Height = 120;
	int Position_x = 0;
	// 0 == top 
	int Position_y = 0;

	QFAViewport* ParentViewport;// if null this unit not root units
	QFAUIParentComponent* Parent;



	virtual void SetSizeParent(unsigned int w, unsigned int h) = 0;
	virtual void SetPositionParent(unsigned int x, unsigned int y) = 0;
	
public:
	virtual void SetPosition(unsigned int x, unsigned int y){}
	virtual void SetSize(unsigned int w, unsigned int h){}

	inline FVector2D GetPosition() 
	{
		return FVector2D(Position_x, Position_y);
	}
	FVector2D GetSize()
	{
		return FVector2D(Width, Height);
	}

	void SetSlot(void* slot);

protected:
	QFAUISlot::SParentSlot Slot;
};

