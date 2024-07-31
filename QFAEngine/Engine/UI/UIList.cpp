#include "pch.h"
#include "UIList.h"


QFAUIList::QFAUIList()
{
	Type = QFAUIType::List;	
}

QFAUIList::~QFAUIList()
{

}

void QFAUIList::WidthChanged(int oldValue)
{
	CalculateChildren(ChildCalculateType::EWidth);
}

void QFAUIList::HeightChanged(int oldValue)
{
	CalculateChildren(ChildCalculateType::EHeight);
}

void QFAUIList::TopChanged(int oldValue)
{
	CalculateChildren(ChildCalculateType::ETop);
}

void QFAUIList::LeftChanged(int oldValue)
{
	CalculateChildren(ChildCalculateType::ELeft);
}

void QFAUIList::SetListType(EListType listType)
{ 
	ListType = listType;
	CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::NewUnit(QFAUIUnit* unit)
{
	CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::MySlotChange(QFAUIUnit* unit)
{
	CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::SetUnitHeight(unsigned int h)
{
	UnitHeight = h;
	if (ListType == LTVertical)
		CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::SetUnitWidth(unsigned int w)
{
	UnitWidth = w;
	if (ListType == LTHorizon)
		CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::CalculateChildren(ChildCalculateType cct)
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
								
				if (cct == ChildCalculateType::ETop || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetPosition_y = Position_y + y ;
					Children[i]->SetTop(Children[i]->StrTop);
				}

				if (cct == ChildCalculateType::ELeft || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetPosition_x = Position_x;
					Children[i]->SetLeft(Children[i]->StrLeft);
				}

				if (cct == ChildCalculateType::EWidth || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetWidth = Width;
					Children[i]->SetWidth(Children[i]->StrWidth, Children[i]->ParentSetWidthMinus);
				}

				if (cct == ChildCalculateType::EHeight || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetHeight = unitHeight;
					Children[i]->SetHeight(Children[i]->StrHeight, Children[i]->ParentSetHeightMinus);
				}
				
				y += unitHeight;
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
					unitWidth = ListType == LTHorizon ? UnitWidth : ((QFAUIParent*)Children[i])->UpdateInnerWidth();
				else
					unitWidth = ListType == LTHorizon ? UnitWidth : Children[i]->InnerWidth;			

				if (cct == ChildCalculateType::ETop || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetPosition_y = Position_y;
					Children[i]->SetTop(Children[i]->StrTop);
				}

				if (cct == ChildCalculateType::ELeft || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetPosition_x = Position_x + x;
					Children[i]->SetLeft(Children[i]->StrLeft);
				}

				if (cct == ChildCalculateType::EWidth || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetWidth = unitWidth;
					Children[i]->SetWidth(Children[i]->StrWidth, Children[i]->ParentSetWidthMinus);
				}

				if (cct == ChildCalculateType::EHeight || cct == ChildCalculateType::EAll)
				{
					Children[i]->ParentSetHeight = Height;
					Children[i]->SetHeight(Children[i]->StrHeight, Children[i]->ParentSetHeightMinus);
				}

				x += unitWidth;
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
			CalculateChildren(ChildCalculateType::EAll);
}

void QFAUIList::UnitWasRemoved()
{
	CalculateChildren(ChildCalculateType::EAll);
}

float QFAUIList::UpdateInnerHeight()
{
	return InnerHeight;
}

float QFAUIList::UpdateInnerWidth()
{
	return InnerWidth;
}