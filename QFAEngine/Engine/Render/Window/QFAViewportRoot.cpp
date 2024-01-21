#include "QFAViewportRoot.h"

QFAViewportRoot::QFAViewportRoot()
{
	IsRoot = true;
	Type = QFAUIType::ViewportRoot;
}

QFAViewportRoot::~QFAViewportRoot()
{
}


void QFAViewportRoot::SetSizeParent(unsigned int w, unsigned int h)
{
	Width = w;
	Height = h;
	for (size_t i = 0; i < Children.Length(); i++)
		Children[i]->SetSizeParent(w, h);
}

void QFAViewportRoot::SetPositionParent(unsigned int x, unsigned int y)
{
	Position_x = x;
	Position_y = y;
	for (size_t i = 0; i < Children.Length(); i++)
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



