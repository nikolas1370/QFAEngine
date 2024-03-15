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
					
					if (i == Children.Length() - 1 && StretchLastUnit)
					{
						if (Height - listSlot->marginTop > y)
						{
							Children[i]->SetSizeParent(Width - listSlot->marginLeft, Height  - listSlot->marginTop - y);
							y += Height - listSlot->marginTop - y;
						}
						else
							Children[i]->SetSizeParent(Width - listSlot->marginLeft, 0);
					}
					else
					{
						Children[i]->SetSizeParent(Width - listSlot->marginLeft, UnitHeight - listSlot->marginTop);
						y += UnitHeight;
					}
					
				}
				else
				{
					QFAUISlot::SListSlot* listSlot = (QFAUISlot::SListSlot*)&Children[i]->Slot;
					Children[i]->SetPositionParent(Position_x + listSlot->marginLeft, Position_y + y + listSlot->marginTop);

					if (i == Children.Length() - 1 && StretchLastUnit)
					{
						if (Height   > y + listSlot->marginTop)
						{
							Children[i]->SetSizeParent(Width - listSlot->marginLeft, Height - (y + listSlot->marginTop));
							y += Height - (y + listSlot->marginTop);
						}
						else 
							Children[i]->SetSizeParent(Width - listSlot->marginLeft, 0);
					}
					else
					{
						Children[i]->SetSizeParent(Width, UnitHeight);
						y += UnitHeight + listSlot->marginTop;
					}


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

					if (i == Children.Length() - 1 && StretchLastUnit)
					{
						if (Width - listSlot->marginLeft > x)
						{
							Children[i]->SetSizeParent(Width - listSlot->marginLeft - x, Height - listSlot->marginTop);
							x += Width - listSlot->marginLeft - x;
						}
						else
							Children[i]->SetSizeParent(0, Height - listSlot->marginTop);
					}
					else
					{
						Children[i]->SetSizeParent(UnitWidth - listSlot->marginLeft, Height - listSlot->marginTop);
						x += UnitWidth;
					}
				}
				else
				{
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
						Children[i]->SetSizeParent(UnitWidth, Height);
						x += UnitWidth + listSlot->marginLeft;
					}
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