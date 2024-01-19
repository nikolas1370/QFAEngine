#include "Canvas.h"
#include <iostream>

QFAUICanvas::QFAUICanvas()
{
	SelfResizable = true;
	Type = EUIType::Canvas;
}

QFAUICanvas::~QFAUICanvas()
{
}

void QFAUICanvas::ViewportSizeChange(int viewportWidth, int viewportHeight)
{
	Width = viewportWidth;
	Height = viewportHeight;
	
	SCanvasSlot* slot;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		slot = (SCanvasSlot*)&Children[i]->Slot.ParentSloot;
		Children[i]->SetPositionParent(slot->x * Width, slot->y * Height);
		Children[i]->SetSizeParent(slot->Width * Width, slot->Height * Height);
	}
}

void QFAUICanvas::NewUnit(QFAUIUnit* unit)
{
	SCanvasSlot* slot = (SCanvasSlot*)&unit->Slot.ParentSloot;
	if (unit->Slot.typeParent != Type)
	{ 
		slot->Width = 1.0f;
		slot->Height = 1.0f;
		slot->x = 0.0f;
		slot->y = 0.0f;		
	}	

	unit->SetPositionParent(slot->x * Width, slot->y * Height);
	unit->SetSizeParent(slot->Width * Width, slot->Height * Height);	
}

void QFAUICanvas::MySlotChange(QFAUIUnit* unit)
{

	NewUnit(unit);
}