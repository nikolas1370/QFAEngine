#include "pch.h"
#include "QFAViewportRoot.h"
#include <Tools/String.h>
#include <Core/EngineStuff/Window/EngineViewport.h>

QFAViewportRoot::QFAViewportRoot()
{
	IsRoot = true;
	Type = QFAUIType::ViewportRoot;
}

QFAViewportRoot::~QFAViewportRoot()
{
}


float QFAViewportRoot::UpdateInnerHeight()
{
	return (float)Height;
}

float QFAViewportRoot::UpdateInnerWidth()
{
	return (float)Width;
}

void QFAViewportRoot::WidthChanged(int oldValue)
{
	if (!Viewport)
		return;

	Width = Viewport->GetSize().X;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetWidth = Width;
		Children[i]->SetWidth(Children[i]->StrWidth, Children[i]->ParentSetWidthMinus);
	}
}

void QFAViewportRoot::HeightChanged(int oldValue)
{
	if (!Viewport)
		return;

	Height = Viewport->GetSize().Y;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetHeight = Height;
		Children[i]->SetHeight(Children[i]->StrHeight, Children[i]->ParentSetHeightMinus);
	}
}

void QFAViewportRoot::TopChanged(int oldValue)
{
	if (!Viewport)
		return;

	Position_y = Viewport->GetPosition().Y;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetPosition_y = Position_y;
		Children[i]->SetTop(Children[i]->StrTop);
	}
}

void QFAViewportRoot::LeftChanged(int oldValue)
{
	if (!Viewport)
		return;

	Position_x = Viewport->GetPosition().X;
	for (size_t i = 0; i < Children.Length(); i++)
	{
		Children[i]->ParentSetPosition_x = Position_x;
		Children[i]->SetLeft(Children[i]->StrLeft);
	}
}

void QFAViewportRoot::NewUnit(QFAUIUnit* unit)
{	
	unit->ParentSetWidth = Width;
	unit->ParentSetHeight = Height;
	unit->ParentSetPosition_x = Position_x;
	unit->ParentSetPosition_y = Position_y;
	unit->SetWidth(unit->StrWidth, unit->ParentSetWidthMinus);
	unit->SetHeight(unit->StrHeight, unit->ParentSetHeightMinus);
	unit->SetTop(unit->StrTop);
	unit->SetLeft(unit->StrLeft);
}

void QFAViewportRoot::MySlotChange(QFAUIUnit* unit)
{
	
}



