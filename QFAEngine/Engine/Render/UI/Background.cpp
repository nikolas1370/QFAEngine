#include "Background.h"

QFAUIBackground::QFAUIBackground()
{	
	Type = QFAUIType::Background;
}

QFAUIBackground::~QFAUIBackground()
{
}

void QFAUIBackground::MySlotChange(QFAUIUnit* unit)
{
}

void QFAUIBackground::ChangeSize(unsigned int w, unsigned int h)
{	
	Height = h;
	Width = w;
	SetChildSize(Child, w, h);
}

void QFAUIBackground::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	SetChildPosition(Child, x, y);
}

float QFAUIBackground::UpdateInnerHeight()
{
	if(!Child->IsValid())
		return 0.0f;

	if (Child->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)Child;
		return parent->UpdateInnerHeight();
	}
	else
		return Child->InnerHeight;
	
}

float QFAUIBackground::UpdateInnerWidth()
{
	if (!Child->IsValid())
		return 0.0f;

	if (Child->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)Child;
		return parent->UpdateInnerWidth();
	}
	else
		return Child->InnerWidth;	
}

void QFAUIBackground::ChangedUnit(QFAUIUnit* unit)
{
	SetChildSize(unit, Width, Height);
	SetChildPosition(unit, Position_x, Position_y);
}