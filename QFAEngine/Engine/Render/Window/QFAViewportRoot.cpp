#include "QFAViewportRoot.h"

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


void QFAViewportRoot::ChangeSize(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	for (size_t i = 0; i < Children.Length(); i++)
		if(Children[i]->SelfResizable)
			Children[i]->SetSizeParent(w, h);
}

void QFAViewportRoot::ChangePosition(int x, int y)
{
	Position_x = x;
	Position_y = y;
	for (size_t i = 0; i < Children.Length(); i++)
		if (Children[i]->SelfResizable)
			Children[i]->SetPositionParent(x, y);	
}

void QFAViewportRoot::NewUnit(QFAUIUnit* unit)
{	
	unit->SetPositionParent(Position_x, Position_y);
	unit->SetSizeParent(Width, Height);	
}

void QFAViewportRoot::MySlotChange(QFAUIUnit* unit)
{
	unit->SetPositionParent(Position_x, Position_y);
	unit->SetSizeParent(Width, Height);
}



