#include "pch.h"
#include "Scroll.h"
#include <Math/Math.h>
#include <UI/RenderUnit.h>
#include "TextInput.h"
#include <Tools/String.h>

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
	
	Child->ParentSetWidth = Width;
	Child->ParentSetHeight = Height;
	Child->SetWidth(Child->StrWidth, Child->ParentSetWidthMinus);
	Child->SetHeight(Child->StrHeight, Child->ParentSetHeightMinus);	
	Child->SetTop(Child->StrTop);
	Child->SetLeft(Child->StrLeft);
	if(Child->CanRender)
		((QFAUIRenderUnit*)Child)->UnitScroll = 0;;

	ScrollValueFinalVertical = 0;
}

void QFAUIScroll::ProcessScroll(double delta,  float addValue, bool shiftDown)
{
	if (!Child) 
	{
		ScrollValueVertical = 0;
		ScrollValueHorizon = 0;
		return;
	}

	if (ScrollType == STVertical)
		ProcessScrollVerticalInside(delta, addValue);
	else if (ScrollType == STHorizon)
		ProcessScrollHorizonInside(delta, addValue);
	else if (ScrollType == STBoth)
	{
		ProcessScrollVerticalInside(delta, shiftDown ? 0.0f : addValue);
		ProcessScrollHorizonInside(delta, shiftDown ? addValue: 0.0f);
	}	
}

void QFAUIScroll::ProcessScrollVerticalInside(double delta, float addValue)
{
	unsigned int childHeight;
	if (Child->CanBeParent)
	{
		if (NotEqualToZero(addValue)) // remove 
			std::cout << addValue << " ProcessScrollHorizonInside" << "\n";

		QFAUIParent* child = (QFAUIParent*)Child;
		childHeight = (unsigned int)child->UpdateInnerHeight();

		if (Child->Type == QFAUIType::TextInput)
		{
			QFAUITextInput* input = (QFAUITextInput*)Child;
			ScrollValueFinalVertical = input->Text->UnitScroll;
		}	
	}
	else
	{
		QFAUIRenderUnit* child = (QFAUIRenderUnit*)Child;
		childHeight = child->InnerHeight < (unsigned int)child->Height ? (unsigned int)child->Height : child->InnerHeight;
	}

	if (Height >= (int)childHeight)
		return;

	if (NotEqualToZero(addValue))
	{
		ScrollValueVertical = ScrollValueToEndVertical;
		ScrollValueVertical += (addValue * ScrollSpeed);
		ScrollValueToEndVertical = ScrollValueVertical;
		ScrollTimeToEndVertical = ScrollTime;
	}
	else if (EqualToZero(ScrollTimeToEndVertical))
		return;
	else if (EqualToZero(ScrollValueVertical))
		return;

	if (delta > ScrollTimeToEndVertical)
	{
		delta = ScrollTimeToEndVertical;
		ScrollTimeToEndVertical = 0.0f;
		ScrollValueVertical = 0.0f;
	}
	else
		ScrollTimeToEndVertical -= delta;

	float pixels = ScrollValueVertical * (float)(delta * Ratio);
	float finalscroll = ScrollValueFinalVertical + pixels;
	if (finalscroll > 0)
	{
		ChangeScrollValueVerticalInside(0.0f);
		ScrollTimeToEndVertical = 0;
		ScrollValueVertical = 0;
		ScrollValueFinalVertical = 0;
		ScrollValueToEndVertical = 0;
		return;
	}
	else if (finalscroll < ((float)childHeight - (float)Height) * -1)
	{
		ChangeScrollValueVerticalInside(((float)childHeight - (float)Height) * -1);
		ScrollTimeToEndVertical = 0;
		ScrollValueVertical = 0;
		ScrollValueToEndVertical = 0;		
		return;
	}

	if (EqualToZero(pixels))
		return;

	AddScrollValueVerticalInside(pixels);
}

void QFAUIScroll::ProcessScrollHorizonInside(double delta, float addValue)
{
	unsigned int childWidth;
	if (Child->CanBeParent)
	{
		if (Child->Type == QFAUIType::TextInput)
		{
			QFAUITextInput* input = (QFAUITextInput*)Child;
			ScrollValueFinalHorizon = input->Text->UnitOffsetX;
		}

		QFAUIParent* child = (QFAUIParent*)Child;
		childWidth = (unsigned int)child->UpdateInnerWidth();
	}
	else
	{
		QFAUIRenderUnit* child = (QFAUIRenderUnit*)Child;
		childWidth = child->InnerWidth < (unsigned int)child->Width ? (unsigned int)child->Width : child->InnerWidth;
	}
	
	if (Width >= (int)childWidth)
		return;	

	if (NotEqualToZero(addValue))
	{
		ScrollValueHorizon = ScrollValueToEndHorizon;
		ScrollValueHorizon += (addValue * ScrollSpeed);
		ScrollValueToEndHorizon = ScrollValueHorizon;
		ScrollTimeToEndHorizon = ScrollTime;
	}
	else if (EqualToZero(ScrollTimeToEndHorizon))
		return;
	else if (EqualToZero(ScrollValueHorizon))
		return;	

	if (delta > ScrollTimeToEndHorizon)
	{
		delta = ScrollTimeToEndHorizon;
		ScrollTimeToEndHorizon = 0.0f;
		ScrollValueHorizon = 0.0f;
	}
	else
		ScrollTimeToEndHorizon -= delta;

	float pixels = ScrollValueHorizon * (float)(delta * Ratio);
	float finalscroll = ScrollValueFinalHorizon + pixels;
	if (finalscroll > 0)
	{
		ChangeScrollValueHorizonInside(0.0f);
		ScrollTimeToEndHorizon = 0;
		ScrollValueHorizon = 0;
		ScrollValueFinalHorizon = 0;
		ScrollValueToEndHorizon = 0;
		return;
	}
	else if (finalscroll < ((float)childWidth - (float)Width) * -1)
	{
		ChangeScrollValueHorizonInside(((float)childWidth - (float)Width) * -1);
		ScrollTimeToEndHorizon = 0;
		ScrollValueHorizon = 0;
		ScrollValueToEndHorizon = 0;
		return;
	}

	if (EqualToZero(pixels))
		return;

	AddScrollValueHorizonInside(pixels);
}


void QFAUIScroll::AddScrollValueVerticalInside(float value)
{
	ScrollValueFinalVertical += value;
	ScrollValueToEndVertical -= value;
	ChangeScrollValueVerticalInside(ScrollValueFinalVertical);
}

void QFAUIScroll::ChangeScrollValueVerticalInside(float value)
{
	float difference = ScrollValueFinalVertical - value;
	ScrollValueFinalVertical = value;
	if (Child->CanBeParent)
	{
		if (Child->Type == QFAUIType::TextInput)
		{
			QFAUITextInput* input = (QFAUITextInput*)Child;
			if ((ScrollValueFinalVertical * -1) >= (input->Text->InnerHeight + difference - input->Height))
				ScrollValueFinalVertical = input->Text->UnitScroll;
			else
				input->Text->UnitScroll = ScrollValueFinalVertical;
		}
		else
		{
			Child->ParentSetPosition_x = Position_x;
			Child->ParentSetPosition_y = Position_y;
			Child->SetTop(Child->StrTop);
			Child->SetLeft(Child->StrLeft);
		}
	}
	else
		((QFAUIRenderUnit*)Child)->UnitScroll = ScrollValueFinalVertical;
}

void QFAUIScroll::AddScrollValueHorizonInside(float value)
{
	ScrollValueFinalHorizon += value;
	ScrollValueToEndHorizon -= value;
	ChangeScrollValueHorizonInside(ScrollValueFinalHorizon);
}

void QFAUIScroll::ChangeScrollValueHorizonInside(float value)
{
	float difference = ScrollValueFinalHorizon - value;
	ScrollValueFinalHorizon = value;
	if (Child->CanBeParent)
	{
		if (Child->Type == QFAUIType::TextInput)
		{
			QFAUITextInput* input = (QFAUITextInput*)Child;
			if ((ScrollValueFinalHorizon * -1) >= (input->Text->InnerWidth + difference - input->Width))
				ScrollValueFinalHorizon = input->Text->UnitOffsetX;
			else
				input->Text->UnitOffsetX = ScrollValueFinalHorizon;
		}
		else
		{
			Child->ParentSetPosition_x = Position_x;
			Child->ParentSetPosition_y = Position_y;
			Child->SetTop(Child->StrTop);
			Child->SetLeft(Child->StrLeft);
		}
	}
		
	else
		((QFAUIRenderUnit*)Child)->UnitOffsetX = ScrollValueFinalHorizon;
}

void QFAUIScroll::NewFrame(QFAUIScroll* scrollUndeCursor, double delta, float addValue, bool shiftDown)
{	
	for (size_t i = 0; i < Scrolls.size(); i++)
	{
		if (Scrolls[i] == scrollUndeCursor)
			scrollUndeCursor->ProcessScroll(delta, addValue, shiftDown);
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

float QFAUIScroll::UpdateInnerWidth()
{
	if (Child->CanBeParent)
	{
		QFAUIParent* child = (QFAUIParent*)Child;
		InnerWidth = (unsigned int)child->UpdateInnerWidth();
	}
	else
		InnerWidth = Child->InnerWidth;

	return (float)InnerWidth;
}


void QFAUIScroll::MySlotChange(QFAUIUnit* unit)
{
}

void QFAUIScroll::WidthChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetWidth = Width;
		Child->SetWidth(Child->StrWidth, Child->ParentSetWidthMinus);
		if (Child->CanRender)
			((QFAUIRenderUnit*)Child)->UnitScroll = ScrollValueFinalVertical;
	}
}

void QFAUIScroll::HeightChanged(int oldValue)
{
	if (Child)
	{
		Child->ParentSetHeight = Height;
		Child->SetHeight(Child->StrHeight, Child->ParentSetHeightMinus);
		if (Child->CanRender)
			((QFAUIRenderUnit*)Child)->UnitScroll = ScrollValueFinalVertical;
	}
}

void QFAUIScroll::TopChanged(int oldValue)
{
	if (Child)
	{
		if (Child->CanBeParent)
		{
			if (Child->Type == QFAUIType::TextInput)
				Child->ParentSetPosition_y = Position_y;
			else
				Child->ParentSetPosition_y = Position_y + (int)ScrollValueFinalVertical;
		}
		else
			Child->ParentSetPosition_y = Position_y;

		Child->SetTop(Child->StrTop);
	}
}

void QFAUIScroll::LeftChanged(int oldValue)
{
	if (Child)
	{
		if (Child->CanBeParent)
		{
			if (Child->Type == QFAUIType::TextInput)
				Child->ParentSetPosition_x = Position_x;
			else
				Child->ParentSetPosition_x = Position_x + (int)ScrollValueFinalHorizon;
		}
		else
			Child->ParentSetPosition_x = Position_x;

		Child->SetLeft(Child->StrLeft);

		
		if (Child->Type == QFAUIType::Grid)
			std::cout << Child->InnerHeight <<  " lox in fish\n";
	}
}
