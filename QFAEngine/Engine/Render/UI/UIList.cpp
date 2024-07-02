#include "pch.h"
#include "UIList.h"

QFAUIList::QFAUIList()
{
	Type = QFAUIType::List;	
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
	if (ListType == LTVertical || ListType == LTVerticalInner)
	{
		int y = 0;
		for (size_t i = 0; i < Children.Length(); i++)
		{
			if (Children[i]->IsValid())
			{
				unsigned int unitHeight;
				if (Children[i]->CanBeParent)
				{
					QFAUIParent* child = (QFAUIParent*)Children[i];
					unitHeight = ListType == LTVertical ? UnitHeight : child->UpdateInnerHeight();
				}
				else
					unitHeight = ListType == LTVertical ? UnitHeight : Children[i]->InnerHeight;
				
				QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
				Children[i]->SetPositionParent(Position_x + listSlot->marginLeft, Position_y + y + listSlot->marginTop);
				if (i == Children.Length() - 1 && StretchLastUnit)
				{
					if (Height > y + listSlot->marginTop)
					{
						Children[i]->SetSizeParent(Width - listSlot->marginLeft, Height - (y + listSlot->marginTop));
						y += Height - (y + listSlot->marginTop);
					}
					else
						Children[i]->SetSizeParent(Width - listSlot->marginLeft, 0);
				}
				else
				{
					Children[i]->SetSizeParent(Width, unitHeight);
					y += unitHeight + listSlot->marginTop;
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
				unsigned int unitWidth;
				if (Children[i]->CanBeParent)
				{
					QFAUIParent* child = (QFAUIParent*)Children[i];
					unitWidth = ListType == LTHorizon ? UnitWidth : child->UpdateInnerWidth();
				}
				else
					unitWidth = ListType == LTHorizon ? UnitWidth : Children[i]->InnerWidth;
				
				QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
				Children[i]->SetPositionParent(Position_x + x + listSlot->marginLeft, Position_y + listSlot->marginTop);
				if (i == Children.Length() - 1 && StretchLastUnit)
				{
					if (Width > x + listSlot->marginLeft)
					{
						Children[i]->SetSizeParent(Width - (listSlot->marginLeft + x), Height - listSlot->marginTop);
						x += Width - (listSlot->marginLeft + x);
					}
					else
						Children[i]->SetSizeParent(0, Height - listSlot->marginTop);
				}
				else
				{
					Children[i]->SetSizeParent(unitWidth, Height);
					x += unitWidth + listSlot->marginLeft;
				}			
			}
		}
		
		InnerHeight = Height;
		InnerWidth = x;
	}
}

void QFAUIList::ChildInnerChange(QFAUIUnit* child)
{
	if (ListType == LTHorizonInner || ListType == LTVerticalInner)
		if (child->Type == QFAUIType::Text || child->Type == QFAUIType::TextInput)
			CalculateChildren();
}

void QFAUIList::UnitWasRemoved()
{
	CalculateChildren();
}

float QFAUIList::UpdateInnerHeight()
{
	return InnerHeight;
}

float QFAUIList::UpdateInnerWidth()
{
	return InnerWidth;
}