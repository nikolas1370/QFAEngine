#include "UIEvent.h"
#include <Render/Window/QFAViewportRoot.h>
#include <Render/Window/Window.h>
#include <Render/UI/Scroll.h>
#include <functional>
#include <Render/Time.h>

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

	Input.AddKeyPress(EKey::MOUSE_LEFT, "LMD", [this](EKey::Key key)
		{
			LeftMouseDown = true;
		});

	Input.AddKeyPress(EKey::MOUSE_RIGHT, "RMD", [this](EKey::Key key)
		{
			RightMouseDown = true;
		});

	Input.AddKeyRelease(EKey::MOUSE_LEFT, "LMU", [this](EKey::Key key)
		{
			LeftMouseUp = true;
		});

	Input.AddKeyRelease(EKey::MOUSE_RIGHT, "RMU", [this](EKey::Key key)
		{
			RightMouseUp = true;
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
	// don't replase int because in "i" can be minus value 
	for (int i = (int)SortUIUnits.Length() - 2; i > 0; i--)
	{
		for (int j = 0; j <= i; j++)
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

void QFAUIEvent::NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, double delta)
{	
	QFAUIUnit* unitUnderFocus = nullptr;
	QFAUIScroll* scrollUnit = nullptr;
	FindUnitUnderFocus(root, unitUnderFocus, scrollUnit, mousePosX, mousePosY);

	ScrollEvent(root, scrollUnit, delta);
	FocusEvent(unitUnderFocus);
	MouseButtonEvent(unitUnderFocus);
}

void QFAUIEvent::FindUnitUnderFocus(QFAViewportRoot* root, QFAUIUnit*& unitUnderFocus, QFAUIScroll*& scrollUnit, float mousePosX, float mousePosY)
{
	if (root)
	{
		SortUIs(root);
		for (size_t i = 0; i < SortUIUnits.Length(); i++)
		{
			float xStart = (float)SortUIUnits[i]->Position_x;
			float yStart = (float)SortUIUnits[i]->Position_y;
			float xEnd = (float)(SortUIUnits[i]->Position_x + SortUIUnits[i]->Width);
			float yEnd = (float)(SortUIUnits[i]->Position_y + SortUIUnits[i]->Height);
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
}

void QFAUIEvent::ScrollEvent( QFAViewportRoot* root, QFAUIScroll* scrollUnit, double delta)
{
	if (scrollUnit && scrollUnit->Type == QFAUIType::Type::Scroll)
		QFAUIScroll::NewFrame(scrollUnit, delta, root ? ScrollValue : 0.0f);

	ScrollValue = 0;
}

void QFAUIEvent::FocusEvent(QFAUIUnit* newUnitUnderFocus)
{
	if (FocusUnit && newUnitUnderFocus)
	{
		if (FocusUnit != newUnitUnderFocus)
		{
			unsigned int lastFocusParentCount;
			unsigned int curentFocusParentCount;
			QFAViewportRoot* rootLast = FocusUnit->GetViewportRoot(lastFocusParentCount);
			QFAViewportRoot* rootCurent = newUnitUnderFocus->GetViewportRoot(curentFocusParentCount);

			if (rootLast == rootCurent)
			{
				QFAUIUnit* last = FocusUnit;
				QFAUIUnit* curent = newUnitUnderFocus;
				while (true) // search common ancestor
				{
					if (lastFocusParentCount == curentFocusParentCount)
					{
						if (last == curent)
						{
							newUnitUnderFocus->NotifyInFocus();
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
					else if (lastFocusParentCount > curentFocusParentCount)
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
				newUnitUnderFocus->NotifyInFocus();
			}
		}
	}
	else if (FocusUnit && !newUnitUnderFocus)
		FocusUnit->NotifyOutFocus(false);
	else if (!FocusUnit && newUnitUnderFocus)
		newUnitUnderFocus->NotifyInFocus();

	FocusUnit = newUnitUnderFocus;
}

void QFAUIEvent::MouseButtonEvent(QFAUIUnit* unitUnderFocus)
{
	if (LeftMouseDown)
	{
		LeftMouseDown = false;
		if (unitUnderFocus)
		{
			unitUnderFocus->NotifyLeftMouseDown();
			LeftMouseUnit = unitUnderFocus;
		}
	}
	else if(LeftMouseUp)
	{
		LeftMouseUp = false;
		if (unitUnderFocus)
		{
			unitUnderFocus->NotifyLeftMouseUp();
			if (LeftMouseUnit == unitUnderFocus)
				LeftMouseUnit->NotifyLeftMouseDownUp();
		}

		LeftMouseUnit = nullptr;
	}

	if (RightMouseDown)
	{
		RightMouseDown = false;
		if (unitUnderFocus)
		{
			unitUnderFocus->NotifyRightMouseDown();
			RightMouseUnit = unitUnderFocus;
		}
	}
	else if(RightMouseUp)
	{
		RightMouseUp = false;
		if (unitUnderFocus)
		{
			unitUnderFocus->NotifyRightMouseUp();
			if (RightMouseUnit == unitUnderFocus)
				unitUnderFocus->NotifyRightMouseDownUp();
		}

		RightMouseUnit = nullptr;
	}
}
