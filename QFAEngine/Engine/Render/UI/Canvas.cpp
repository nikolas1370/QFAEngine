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

	unit->SetPositionParent((int)(slot->x * (float)Width + (float)Position_x),
							(int)(slot->y * (float)Height + (float)Position_y));
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
		Children[i]->SetPositionParent((int)(slot->x * (float)Width + (float)Position_x),
									   (int)(slot->y * (float)Height + (float)Position_y));
		Children[i]->SetSizeParent((int)(slot->Width * (float)Width), 
								   (int)(slot->Height * (float)Height));
	}
}

float QFAUICanvas::UpdateInnerHeight()
{	
	return (float)Height;
}

