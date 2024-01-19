#pragma once

#include <cstring>

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
	enum EUIType : unsigned char
	{
		NONE = 0,
		Text = 1,
		Canvas = 2
	};

	bool CanBeParent = false;
	EUIType Type = EUIType::NONE;
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

	// use for parent slot
	struct SParentSlot
	{		
		float ParentSloot[4];
		QFAUIUnit::EUIType typeParent;
	};

	/*
	
	do with template
	
	*/
	void SetSlot(void* slot, int slotSize);

protected:
	SParentSlot Slot;
};

