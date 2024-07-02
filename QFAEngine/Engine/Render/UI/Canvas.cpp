#include "pch.h"
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

	unit->SetSizeParent((int)(slot->WidthInPixel ? slot->Width : slot->Width * (float)Width),
		(int)(slot->HeightInPixel ? slot->Height : slot->Height * (float)Height));
	unit->SetPositionParent((int)(slot->xInPixel ? slot->x + (float)Position_x : slot->x * (float)Width + (float)Position_x),
							(int)(slot->yInPixel ? slot->y + (float)Position_y : slot->y * (float)Height + (float)Position_y));	
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
		Children[i]->SetSizeParent((int)(slot->WidthInPixel ? slot->Width : slot->Width * (float)Width),
			(int)(slot->HeightInPixel ? slot->Height : slot->Height * (float)Height));

		Children[i]->SetPositionParent((int)(slot->xInPixel ? slot->x + (float)Position_x : slot->x * (float)Width + (float)Position_x),
									   (int)(slot->yInPixel ? slot->y + (float)Position_y : slot->y * (float)Height + (float)Position_y));		
	}
}

float QFAUICanvas::UpdateInnerHeight()
{	
	return (float)Height;
}

float QFAUICanvas::UpdateInnerWidth()
{
	return (float)Width;
}

