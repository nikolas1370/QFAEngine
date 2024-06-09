#include "SelectUnit.h"
#include <Render/Time.h>

std::vector<QFAUISelectUnit*> QFAUISelectUnit::SelectUnitList;

void QFAUISelectUnit::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	SetChildSize(&Scroll, w, h);
}

void QFAUISelectUnit::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(&Scroll, x, y);
}

float QFAUISelectUnit::UpdateInnerHeight()
{
	return Height;
}

float QFAUISelectUnit::UpdateInnerWidth()
{
	return Width;
}

void QFAUISelectUnit::SetSelectUnit(QFAUIParent* unit)
{
	if (!unit)
	{
		SelectedUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
		SelectedUnit = nullptr;
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

	std::cout << "lox\n";
}

void QFAUISelectUnit::SetScrollChild(QFAUIParentMultipleUnit* child)
{
	SelectUnitChild = child;
	AddHiddenChild(&Scroll);
	Scroll.SetUnit(child);

	Events.SetInFocus(QFAUISelectUnit::InFocus, this);
	Events.SetOutFocus(QFAUISelectUnit::OutFocus, this);
	Events.SetLeftMouseDown(QFAUISelectUnit::LeftMouseDown, this);
}

void QFAUISelectUnit::InFocus(QFAUIUnit* unit, void* _this)
{
	QFAUISelectUnit* su = (QFAUISelectUnit*)_this;
	if (su->FocusUnit)
	{
		su->FocusUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
		su->FocusUnit = nullptr;
	}

	if (unit == su || unit == &su->Scroll || unit == su->SelectUnitChild)
	{
		if (su->SelectEvent.InFocus)
			su->SelectEvent.InFocus(su);

		return;
	}
	
	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent || !parent->GetParent())
			return;
		else if (parent->GetParent() == su->SelectUnitChild) 
		{			
			if (parent != su->SelectedUnit)
			{	
				su->FocusUnit = (QFAUIParent*)parent;
				su->FocusUnit->SetBackgroundColor(su->FocusColor);
			}			

			if (su->SelectEvent.InFocus)
				su->SelectEvent.InFocus((QFAUIParent*)parent);
			
			return;
		}

		parent = parent->GetParent();
	}
}

void QFAUISelectUnit::OutFocus(void* _this)
{
	QFAUISelectUnit* su = (QFAUISelectUnit*)_this;		
	if (su->FocusUnit)
	{
		su->FocusUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));
		su->FocusUnit = nullptr;
	}

	if (su->SelectEvent.OutFocus)
		su->SelectEvent.OutFocus();
}

void QFAUISelectUnit::LeftMouseDown(QFAUIUnit* unit, void* _this)
{
	QFAUISelectUnit* su = (QFAUISelectUnit*)_this;
	if (unit == su || unit == &su->Scroll || unit == su->SelectUnitChild)
	{
		su->LastClickUnit = nullptr;
		su->LastClickTime = 0;
		if(su->SelectedUnit)
		{
			su->SelectedUnit->SetBackgroundColor(QFAColor(0,0,0,0));
			su->SelectedUnit = nullptr;
		}

		if (su->SelectEvent.LeftMouseDown)
			su->SelectEvent.LeftMouseDown(su);

		return;
	}
	
	QFAUIUnit* parent = unit;
	while (true)
	{
		if (!parent || !parent->GetParent())
			return;
		else if (parent->GetParent() == su->SelectUnitChild)
		{			
			if (su->SelectedUnit)
				su->SelectedUnit->SetBackgroundColor(QFAColor(0, 0, 0, 0));

			su->FocusUnit = nullptr;
			su->SelectedUnitFocus = true;
			su->SelectedUnit = (QFAUIParent*)parent;
			su->SelectedUnit->SetBackgroundColor(su->SelectColor);			
			if (su->SelectEvent.LeftMouseDown)
				su->SelectEvent.LeftMouseDown(su->SelectedUnit);

			if (su->LastClickUnit)
			{
				if ((QTime::GetTime() - su->LastClickTime) < su->DobleClickTime && 
					su->SelectEvent.DobleClick && su->LastClickUnit == parent)
						su->SelectEvent.DobleClick(su->SelectedUnit);

				su->LastClickTime = 0;
				su->LastClickUnit = nullptr;
			}
			else
			{
				su->LastClickTime = QTime::GetTime();
				su->LastClickUnit = (QFAUIParent*)parent;
			}

			return;
		}

		parent = parent->GetParent();
	}
}

void QFAUISelectUnit::WindowLeftMouseDown(QFAWindow* window, QFAUIUnit* unitUnderFocus)
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
