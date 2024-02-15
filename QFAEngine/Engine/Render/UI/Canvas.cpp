#include "Canvas.h"
#include <iostream>

QFAUICanvas::QFAUICanvas()
{
	SelfResizable = true;
	Type = QFAUIType::Canvas;
}

QFAUICanvas::~QFAUICanvas()
{
}





void QFAUICanvas::NewUnit(QFAUIUnit* unit)
{
	QFAUISlot::SCanvasSlot* slot = (QFAUISlot::SCanvasSlot*)&unit->Slot;
	if (unit->Slot.BaseInfo.typeParent != Type)
	{ 
		slot->Width = 1.0f;
		slot->Height = 1.0f;
		slot->x = 0.0f;
		slot->y = 0.0f;		
	}	

	unit->SetPositionParent(slot->x * Width + Position_x, slot->y * Height + Position_y);
	unit->SetSizeParent(slot->Width * Width, slot->Height * Height);	
}

void QFAUICanvas::MySlotChange(QFAUIUnit* unit)
{
	NewUnit(unit);
}

void QFAUICanvas::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;

	UpdatePositionSizeChildren();	
}

void QFAUICanvas::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	UpdatePositionSizeChildren();
}

void QFAUICanvas::UpdatePositionSizeChildren()
{
	QFAUISlot::SCanvasSlot* slot;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		slot = (QFAUISlot::SCanvasSlot*)&Children[i]->Slot;
		Children[i]->SetPositionParent(slot->x * Width + Position_x, slot->y * Height + Position_y);
		Children[i]->SetSizeParent(slot->Width * Width, slot->Height * Height);
	}
}

float QFAUICanvas::UpdateInnerHeight()
{	
	return Height;
}

