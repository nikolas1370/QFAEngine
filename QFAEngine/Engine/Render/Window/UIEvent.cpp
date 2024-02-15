#include "UIEvent.h"
#include <Render/Window/QFAViewportRoot.h>
#include <Render/Window/Window.h>
#include <Render/UI/Scroll.h>
#include <functional>

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
	QFAUIUnit* unitUnderFocus = nullptr;
	QFAUIScroll* scrollUnit = nullptr;
	if (root)
	{
		SortUIs(root);	
		for (size_t i = 0; i < SortUIUnits.Length(); i++)
		{
			float xStart = SortUIUnits[i]->Position_x;
			float yStart = SortUIUnits[i]->Position_y;
			float xEnd = SortUIUnits[i]->Position_x + SortUIUnits[i]->Width;
			float yEnd = SortUIUnits[i]->Position_y + SortUIUnits[i]->Height;
			QFAUIUnit::UniformOverflow over;
			over.enable = 0;
			over.leftTopX = 0;
			over.leftTopY = 0;
			over.rightBottomX = 1000000;
			over.rightBottomY = 1000000;

			SortUIUnits[i]->ProcessParentOverflow(over, SortUIUnits[i]->Parent);
			if (over.leftTopX > xStart)
				xStart = over.leftTopX;

			if (over.leftTopY > yStart)
				yStart = over.leftTopY;

			if (over.rightBottomX < xEnd)
				xEnd = over.rightBottomX;

			if (over.rightBottomY < yEnd)
				yEnd = over.rightBottomY;

			if (mousePosX >= xStart && mousePosY >= yStart &&
				mousePosX <= xEnd && mousePosY <= yEnd)
			{

				if (!unitUnderFocus)
				{
					unitUnderFocus = SortUIUnits[i];
					if (unitUnderFocus && scrollUnit)
						break;
				}
				
				if (!scrollUnit && SortUIUnits[i]->Type == QFAUIType::Scroll)
				{
					scrollUnit = (QFAUIScroll*)SortUIUnits[i];
					if (unitUnderFocus && scrollUnit)
						break;
				}				
			}
		}
	}

	if (FocusUnit && unitUnderFocus)
	{
		if (FocusUnit != unitUnderFocus)
		{
			unsigned int lastFocusParentCount;
			unsigned int curentFocusParentCount;
			QFAViewportRoot* rootLast = FocusUnit->GetViewportRoot(lastFocusParentCount);
			QFAViewportRoot* rootCurent = unitUnderFocus->GetViewportRoot(curentFocusParentCount);

			if (rootLast == rootCurent)
			{
				QFAUIUnit* last = FocusUnit;
				QFAUIUnit* curent = unitUnderFocus;				
				while (true) // search common ancestor
				{					
					if (lastFocusParentCount == curentFocusParentCount)
					{						
						if (last == curent)
						{										
							unitUnderFocus->NotifyInFocus();
							last = FocusUnit;
							while (true)
							{
								if (last == curent)
									break;

								last->NotifyOutFocus(true);
								last = last->Parent;
							}

							break;
						}
						else
						{
							last = last->Parent;
							curent = curent->Parent;
						}
					}
					else if(lastFocusParentCount > curentFocusParentCount)
					{
						last = last->Parent;
						lastFocusParentCount--;
					}
					else if (lastFocusParentCount < curentFocusParentCount)
					{
						curent = curent->Parent;
						curentFocusParentCount--;
					}
				}
			}
			else 
			{
				FocusUnit->NotifyOutFocus(false);
				unitUnderFocus->NotifyInFocus();
			}
		}		
	}
	else if(FocusUnit && !unitUnderFocus)
		FocusUnit->NotifyOutFocus(false);
	else if (!FocusUnit && unitUnderFocus)
		unitUnderFocus->NotifyInFocus();
	
	FocusUnit = unitUnderFocus;
	if(scrollUnit && scrollUnit->Type == QFAUIType::Type::Scroll)
		QFAUIScroll::NewFrame(scrollUnit, delta, root ? ScrollValue : 0.0f);

	ScrollValue = 0;
}
