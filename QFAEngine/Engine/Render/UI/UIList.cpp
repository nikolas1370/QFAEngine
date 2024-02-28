#include "UIList.h"

QFAUIList::QFAUIList()
{
	Type = QFAUIType::List;
	Overflow = QFAUIParent::Hidden;
}

QFAUIList::~QFAUIList()
{

}

void QFAUIList::SetListType(EListType listType)
{ 
	ListType = listType;
	CalculateChildren();
}

void QFAUIList::NewUnit(QFAUIUnit* unit)
{
	CalculateChildren();
}

void QFAUIList::MySlotChange(QFAUIUnit* unit)
{
	CalculateChildren();
}


void QFAUIList::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	CalculateChildren();
}

void QFAUIList::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	CalculateChildren();
}



void QFAUIList::SetUnitHeight(unsigned int h)
{
	UnitHeight = h;
	if (ListType == LTVertical)
		CalculateChildren();
}

void QFAUIList::SetUnitWidth(unsigned int w)
{
	UnitWidth = w;
	if (ListType == LTHorizon)
		CalculateChildren();
}

void QFAUIList::CalculateChildren()
{
	/*

SListSlot
	{
		SSlotBaseInfo BaseInfo{ QFAUIType::List, sizeof(QFAUISlot::SListSlot) };
		int marginLeft;
		int marginTop;
*/

	
	if (ListType == LTVertical)
	{
		int y = 0;
		for (size_t i = 0; i < Children.Length(); i++)
		{
			if (Children[i]->IsValid())
			{
				if (marginMinus)
				{
					QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
					Children[i]->SetPositionParent(Position_x + listSlot->marginLeft, Position_y + y + listSlot->marginTop);
					Children[i]->SetSizeParent(Width - listSlot->marginLeft, UnitHeight - listSlot->marginTop);
					y += UnitHeight;
				}
				else
				{
					QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
					Children[i]->SetPositionParent(Position_x + listSlot->marginLeft, Position_y + y + listSlot->marginTop);
					Children[i]->SetSizeParent(Width, UnitHeight);
					y += UnitHeight + listSlot->marginTop;
				}				
			}
		}

		InnerHeight = y;
		InnerWidth = Width;
	}
	else
	{
		int x = 0;
		for (size_t i = 0; i < Children.Length(); i++)
		{
			if (Children[i]->IsValid())
			{
				if (marginMinus)
				{
					QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
					Children[i]->SetPositionParent(Position_x + x + listSlot->marginLeft, Position_y + listSlot->marginTop);
					Children[i]->SetSizeParent(UnitWidth - listSlot->marginLeft, Height - listSlot->marginTop);
					x += UnitWidth;
				}
				else
				{
					QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
					Children[i]->SetPositionParent(Position_x + x + listSlot->marginLeft, Position_y + listSlot->marginTop);
					Children[i]->SetSizeParent(UnitWidth, Height);
					x += UnitWidth + listSlot->marginLeft;
				}				
			}
		}
		
		InnerHeight = Height;
		InnerWidth = x;
	}
}


float QFAUIList::UpdateInnerHeight()
{
	return InnerHeight;
}

float QFAUIList::UpdateInnerWidth()
{
	return InnerWidth;
}