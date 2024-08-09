#include "pch.h"
#include "SelectUnit.h"
#include <Overlord/Time.h>
#include <Tools/String.h>

QFAUISelectUnit::QFAUISelectUnit()
{
	Type = QFAUIType::SelectUnit;
	SelectUnitList.push_back(this);
}

QFAUISelectUnit::~QFAUISelectUnit()
{
	for (size_t i = 0; i < SelectUnitList.size(); i++)
	{
		if (SelectUnitList[i] == this)
		{
			SelectUnitList.erase(SelectUnitList.begin() + i);
			break;
		}
	}
}

std::vector<QFAUISelectUnit*> QFAUISelectUnit::SelectUnitList;

float QFAUISelectUnit::UpdateInnerHeight()
{
	return Height;
}

float QFAUISelectUnit::UpdateInnerWidth()
{
	return Width;
}

void QFAUISelectUnit::WidthChanged(int oldValue)
{
	Scroll.ParentSetWidth = Width;
	Scroll.SetWidth("100%");
}

void QFAUISelectUnit::HeightChanged(int oldValue)
{
	Scroll.ParentSetHeight = Height;
	Scroll.SetHeight("100%");
}

void QFAUISelectUnit::TopChanged(int oldValue)
{
	Scroll.ParentSetPosition_y = Position_y;
	Scroll.SetTop(nullptr);
}

void QFAUISelectUnit::LeftChanged(int oldValue)
{
	Scroll.ParentSetPosition_x = Position_x;
	Scroll.SetLeft(nullptr);
}

void QFAUISelectUnit::SetSelectUnit(QFAUIParent* unit)
{
	if (!unit)
	{
		if (SelectedUnit)
		{
			SelectedUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
			SelectedUnit = nullptr;
		}
		return;
	}	

	for (size_t i = 0; i < SelectUnitChild->Children.Length(); i++)
	{
		if (SelectUnitChild->Children[i] == unit)
		{
			LastClickUnit = nullptr;
			if (SelectedUnit)
				SelectedUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));

			SelectedUnit = unit;
			SelectedUnit->SetBackgroundColor(SelectColor);
			return;
		}
	}
}

void QFAUISelectUnit::SetScrollChild(QFAUIParentMultipleUnit* child)
{	
	SelectUnitChild = child;
	AddHiddenChild(&Scroll);
	Scroll.SetUnit(child);

	SetInFocus();
	SetOutFocus();
	SetLeftMouseDown();	
}

void QFAUISelectUnit::SetInFocus()
{
	Events.SetInFocus([this](QFAUIUnit* unit)
	{
		if (FocusUnit)
		{
			FocusUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
			FocusUnit = nullptr;
		}

		if (unit == this || unit == &Scroll || unit == SelectUnitChild)
		{
			LastClickUnit = nullptr;
			if (SelectEvent.InFocus)
				SelectEvent.InFocus(this);

			return;
		}

		QFAUIUnit* parent = unit;
		while (true)
		{
			if (!parent || !parent->GetParent())
				return;
			else if (parent->GetParent() == SelectUnitChild)
			{
				if (parent != SelectedUnit)
				{
					LastClickUnit = nullptr;
					FocusUnit = (QFAUIParent*)parent;
					FocusUnit->SetBackgroundColor(FocusColor);
				}

				if (SelectEvent.InFocus)
					SelectEvent.InFocus((QFAUIParent*)parent);

				return;
			}

			parent = parent->GetParent();
		}
	});	
}

void QFAUISelectUnit::SetOutFocus()
{
	Events.SetOutFocus([this]()
	{
		if (FocusUnit)
		{
			FocusUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
			FocusUnit = nullptr;
		}

		if (SelectEvent.OutFocus)
			SelectEvent.OutFocus();
	});
}

void QFAUISelectUnit::SetLeftMouseDown()
{
	Events.SetLeftMouseDown([this](QFAUIUnit* unit)
	{
		if (unit == this || unit == &Scroll || unit == SelectUnitChild)
		{
			LastClickUnit = nullptr;
			LastClickTime = 0;
			if(SelectedUnit)
			{
				SelectedUnit->SetBackgroundColor(QFAColor(0,0,0,0));
				SelectedUnit = nullptr;
			}

			if (SelectEvent.LeftMouseDown)
				SelectEvent.LeftMouseDown(this);		
			return;
		}
		
		QFAUIUnit* parent = unit;
		while (true)
		{
			if (!parent || !parent->GetParent())
				return;
			else if (parent->GetParent() == SelectUnitChild)
			{						
				if (SelectedUnit)
					SelectedUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));

				FocusUnit = nullptr;
				SelectedUnitFocus = true;
				SelectedUnit = (QFAUIParent*)parent;
				SelectedUnit->SetBackgroundColor(SelectColor);			
				if (SelectEvent.LeftMouseDown)
					SelectEvent.LeftMouseDown(SelectedUnit);

				if (LastClickUnit)
				{
					if ((QTime::GetTime() - LastClickTime) < DobleClickTime && 
						SelectEvent.DobleClick && LastClickUnit == parent)
					{
						LastClickUnit = nullptr;
						SelectEvent.DobleClick(SelectedUnit);
					}
					else
					{
						LastClickTime = QTime::GetTime();
						LastClickUnit = (QFAUIParent*)parent;
					}				
				}
				else
				{
					LastClickTime = QTime::GetTime();
					LastClickUnit = (QFAUIParent*)parent;
				}

				return;
			}

			parent = parent->GetParent();
		}
	});	
}

void QFAUISelectUnit::ChildUnderDelete(QFAUIUnit* child)
{
	if (FocusUnit == child)
		FocusUnit = nullptr;
}

void QFAUISelectUnit::WindowLeftMouseDown(QFAEngineWindow* window, QFAUIUnit* unitUnderFocus)
{
	for (size_t i = 0; i < SelectUnitList.size(); i++)
	{
		if (SelectUnitList[i]->GetWindow() == window)
		{
			if (unitUnderFocus && unitUnderFocus->IsMyParent(SelectUnitList[i]))
				break;

			if (SelectUnitList[i]->SelectedUnit)
			{
				SelectUnitList[i]->SelectedUnitFocus = false;
				SelectUnitList[i]->SelectedUnit->SetBackgroundColor(SelectUnitList[i]->SelectLostFocusColor);
			}
		}
	}
}
