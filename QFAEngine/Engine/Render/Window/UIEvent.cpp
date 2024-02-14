#include "UIEvent.h"
#include <Render/Window/QFAViewportRoot.h>
#include <Render/Window/Window.h>
#include <Render/UI/Scroll.h>

QFAUIEvent::QFAUIEvent()
{
}

QFAUIEvent::~QFAUIEvent()
{
}

void QFAUIEvent::Init(QFAWindow* window)
{
	Window = window;
	Input.SetWheelAxis([this](float axis)
		{
			this->ScrollValue = axis;			
		});
}

void QFAUIEvent::AddUnitToSortList(QFAUIUnit* unit)
{
	if (!unit)
		return;

	if (!unit->IsEnable)
		return;

	if (unit->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)unit;
		if (parent->OneUnit)
			AddUnitToSortList(((QFAUIParentOneUnit*)unit)->Child);
		else
		{
			QFAUIParentMultipleUnit* parent = (QFAUIParentMultipleUnit*)unit;
			for (size_t i = 0; i < parent->Children.Length(); i++)
				AddUnitToSortList(parent->Children[i]);
		}		
	}

	SortUIUnits.Add(unit);// parent should add after children
}

void QFAUIEvent::SortUIs(QFAViewportRoot* root)
{
	SortUIUnits.Clear();
	for (size_t i = 0; i < root->Children.Length(); i++)
		AddUnitToSortList(root->Children[i]);

	for (int i = SortUIUnits.Length() - 2; i > 0; i--)
	{
		for (size_t j = 0; j <= i; j++)
		{ // in start of array number smaller
			if (SortUIUnits[j]->ZIndex > SortUIUnits[j + 1]->ZIndex) // ZIndex forward == 1 
			{
				QFAUIUnit* pTem = SortUIUnits[j];
				SortUIUnits[j] = SortUIUnits[j + 1];
				SortUIUnits[j + 1] = pTem;
			}
		}
	}
}



void QFAUIEvent::NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, float delta)
{	
	QFAUIScroll* curentScroll = nullptr;
	if (root)
	{
		SortUIs(root);	
		for (size_t i = 0; i < SortUIUnits.Length(); i++)
		{
			if (SortUIUnits[i]->Type == QFAUIType::Scroll)
			{
				float xEnd = SortUIUnits[i]->Position_x + SortUIUnits[i]->Width;
				float yEnd = SortUIUnits[i]->Position_y + SortUIUnits[i]->Height;
				if (mousePosX >= SortUIUnits[i]->Position_x && mousePosY >= SortUIUnits[i]->Position_y &&
					mousePosX <= xEnd && mousePosY <= yEnd)
				{
					curentScroll = (QFAUIScroll*)SortUIUnits[i];
					break;
				}
			}
		}
	}

	QFAUIScroll::NewFrame(curentScroll, delta, root ? ScrollValue : 0.0f);
	ScrollValue = 0;
}
