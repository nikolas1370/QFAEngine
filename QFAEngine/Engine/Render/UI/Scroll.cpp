#include "Scroll.h"
#include <Math/Math.h>
#include <Render/UI/RenderUnit.h>

std::vector<QFAUIScroll*> QFAUIScroll::Scrolls;

QFAUIScroll::QFAUIScroll()
{
	Type = QFAUIType::Scroll;
	Overflow = EOverflow::Hidden;	

	Scrolls.push_back(this);
}

QFAUIScroll::~QFAUIScroll()
{
	for (size_t i = 0; i < Scrolls.size(); i++)
	{
		if (Scrolls[i] == this)
		{
			Scrolls.erase(Scrolls.begin() + i);
			break;
		}
	}
}

void QFAUIScroll::ChangedUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;
	
	Child->SetPositionParent(Position_x, Position_y);	
	if(Child->CanRender)
		((QFAUIRenderUnit*)Child)->UnitScroll = 0;;

	ScrollValueFinal = 0;
}

void QFAUIScroll::ProcessScroll(double delta,  float addValue)
{
	if (!Child) 
	{
		ScrollValue = 0;
		return;
	}

	unsigned int childHeight;

	if (Child->CanBeParent)
	{
		QFAUIParent* child = (QFAUIParent*)Child;
		childHeight = (unsigned int)child->UpdateInnerHeight();
	}
	else
	{
		QFAUIRenderUnit * child = (QFAUIRenderUnit*)Child;
		childHeight = child->InnerHeight < (unsigned int)child->Height ? (unsigned int)child->Height : child->InnerHeight;
	}

	if (Height >= (int)childHeight)
		return;

	if (NotEqualToZero(addValue))
	{
		ScrollValue = ScrollValueToEnd;
		ScrollValue += (addValue * ScrollSpeed);
		ScrollValueToEnd = ScrollValue;
		ScrollTimeToEnd = ScrollTime;
	}
	else if (EqualToZero(ScrollTimeToEnd))
		return;
	else if (EqualToZero(ScrollValue))
		return;

	if (delta > ScrollTimeToEnd)
	{
		delta = ScrollTimeToEnd;
		ScrollTimeToEnd = 0.0f;
		ScrollValue = 0.0f;
	}
	else
		ScrollTimeToEnd -= delta;

	float pixels = ScrollValue * (float)(delta * Ratio);
	float finalscroll = ScrollValueFinal + pixels;	 
	if (finalscroll > 0)
	{
		ChangeScrollValueInside(0.0f);
		ScrollTimeToEnd = 0;
		ScrollValue = 0;
		ScrollValueFinal = 0;
		ScrollValueToEnd = 0;
		return;
	}
	else if (finalscroll < ((float)childHeight - (float)Height) * -1)
	{
		ChangeScrollValueInside(((float)childHeight - (float)Height) * -1);
		ScrollTimeToEnd = 0;
		ScrollValue = 0;
		ScrollValueToEnd = 0;
		return;
	}
	
	if (EqualToZero(pixels))
		return;

	AddScrollValueInside(pixels);
}


void QFAUIScroll::AddScrollValueInside(float value)
{
	ScrollValueFinal += value;
	ScrollValueToEnd -= value;
	ChangeScrollValueInside(ScrollValueFinal);
}

void QFAUIScroll::ChangeScrollValueInside(float value)
{
	ScrollValueFinal = value;
	if (Child->CanBeParent)
		SetPositionParent(Position_x, Position_y);
	else
		((QFAUIRenderUnit*)Child)->UnitScroll = ScrollValueFinal;
}

void QFAUIScroll::NewFrame(QFAUIScroll* scrollUndeCursor, double delta, float addValue)
{
	for (size_t i = 0; i < Scrolls.size(); i++)
	{
		if (Scrolls[i] == scrollUndeCursor)
			scrollUndeCursor->ProcessScroll(delta, addValue);
		else
			Scrolls[i]->ProcessScroll(delta);
	}
}

float QFAUIScroll::UpdateInnerHeight()
{
	if (Child->CanBeParent)
	{
		QFAUIParent* child = (QFAUIParent*)Child;
		InnerHeight = (unsigned int)child->UpdateInnerHeight();
	}
	else
		InnerHeight = Child->InnerHeight;

	return (float)InnerHeight;
}

void QFAUIScroll::MySlotChange(QFAUIUnit* unit)
{
}

void QFAUIScroll::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	if (Child)
	{
		Child->SetSizeParent(w, h);
		if (Child->CanRender)
		{
			((QFAUIRenderUnit*)Child)->UnitScroll = ScrollValueFinal;
		}
	}
}

void QFAUIScroll::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	if (Child)
	{
		if (Child->CanBeParent)
			((QFAUIParent*)Child)->SetPositionParent(x, y + (int)ScrollValueFinal);
		else
			((QFAUIRenderUnit*)Child)->SetPositionParent(x, y);
	}
}