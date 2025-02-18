#include "pch.h"
#include "UIEvent.h"
#include <Core/EngineStuff/Window/QFAViewportRoot.h>
#include <Core/EngineStuff/Window/EngineWindow.h>
#include <UI/Scroll.h>
#include <functional>
#include <Core/Time.h>
#include <UI/TextInput.h>
#include <UI/UIParentHiddenChild.h>
#include <UI/SelectUnit.h>
#include <GLFW/glfw3.h>

std::vector<QFAUIEvent*> QFAUIEvent::Events;

QFAUIEvent::QFAUIEvent(QFAEngineWindow* window, GLFWwindow* _glfWindow)
{
	Window = window;
	glfWindow = _glfWindow;
	Events.push_back(this);
	Input = new QFAInput(window);
	Input->SetWheelAxis([this](float axis)
		{
			this->ScrollValue = axis;
		});

	Input->AddKeyPress(EKey::MOUSE_LEFT, "LMD", [this](EKey::Key key)
		{
			LeftMouseDown = true;
		});

	Input->AddKeyPress(EKey::MOUSE_RIGHT, "RMD", [this](EKey::Key key)
		{
			RightMouseDown = true;
		});

	Input->AddKeyPress(EKey::MOUSE_5, "FMD", [this](EKey::Key key)
		{
			ForwardMouseDown = true;
		});

	Input->AddKeyPress(EKey::MOUSE_4, "BMD", [this](EKey::Key key)
		{
			BackwardMouseDown = true;
		});

	Input->AddKeyRelease(EKey::MOUSE_LEFT, "LMU", [this](EKey::Key key)
		{
			LeftMouseUp = true;
		});

	Input->AddKeyRelease(EKey::MOUSE_RIGHT, "RMU", [this](EKey::Key key)
		{
			RightMouseUp = true;
		});

	
	if(glfWindow) // if parent window si regular window
		glfwSetCharCallback(glfWindow, QFAUIEvent::CharCallback);
	else if(window->ParentWindow)
	{
		window->ParentWindow->AddCharCallback(window, [this](GLFWwindow* window, unsigned int codepoint)
		{
			if (TextInput->IsValid())
				TextInput->AddChar(codepoint); 
		});
	}
	
	Input->AddKeyPress(EKey::LEFT, "left", [this](EKey::Key key)
		{
			if (this->TextInput->IsValid())
				this->TextInput->PenLeft();
		});

	Input->AddKeyPress(EKey::RIGHT, "right", [this](EKey::Key key)
		{
			if (this->TextInput->IsValid())
				this->TextInput->PenRight();
		});

	Input->AddKeyPress(EKey::BACKSPACE, "BACKSPACE", [this](EKey::Key key)
		{
			if (this->TextInput->IsValid())
				this->TextInput->RemoveChar();
		});

	Input->AddKeyRelease(EKey::ENTER, "Enter", [this](EKey::Key key)
		{
			if (TextInput)
			{
				TextInput->OutInputfocus();
				TextInput = nullptr;
			}
		});

	Input->AddKeyPress(EKey::LEFT_SHIFT, "DLSH", [this](EKey::Key key)
		{
			LeftShiftDown = true;
		});

	Input->AddKeyRelease(EKey::LEFT_SHIFT, "ULSH", [this](EKey::Key key)
		{
			LeftShiftDown = false;
		});

	Input->AddKeyPress(EKey::RIGHT_SHIFT, "DRSH", [this](EKey::Key key)
		{
			RightShiftDown = true;
		});

	Input->AddKeyRelease(EKey::RIGHT_SHIFT, "URSH", [this](EKey::Key key)
		{
			RightShiftDown = false;
		});
}

QFAUIEvent::~QFAUIEvent()
{
	delete Input;
	auto ite = std::find(Events.begin(), Events.end(), this);
	if(ite != Events.end())
		Events.erase(ite);
}

void QFAUIEvent::AddUnitToSortList(QFAUIUnit* unit)
{
	if (!unit)
		return;

	if (!unit->IsEnable)
		return;

	SortUIUnits.Add(unit);
	if (unit->CanBeParent)
	{
		QFAUIParent* parent = (QFAUIParent*)unit;
		if (parent->GetParentType() == QFAUIParent::EParentType::OneChild)
			AddUnitToSortList(((QFAUIParentOneUnit*)unit)->Child);	
		else if (parent->GetParentType() == QFAUIParent::EParentType::MultipleChild)
		{
			QFAUIParentMultipleUnit* multipleParent = (QFAUIParentMultipleUnit*)unit;
			for (size_t i = 0; i < multipleParent->Children.Length(); i++)
				AddUnitToSortList(multipleParent->Children[i]);
		}// if parent type HiddenChild do nothing
		else if (parent->GetParentType() == QFAUIParent::EParentType::HiddenChild)
		{
			if (parent->Type != QFAUIType::TextInput) // TextInput not give own child
			{
				QFAParentHiddenChild* hidenPArent = (QFAParentHiddenChild*)parent;
				for (size_t i = 0; i < hidenPArent->Children.size(); i++)
					AddUnitToSortList(hidenPArent->Children[i]);
			}			
		}
		else 
			stopExecute("QFAUIEvent::AddUnitToSortList undefined parent type");
	}	
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

void QFAUIEvent::NewFrame(QFAViewportRoot* root, float mousePosX, float mousePosY, double delta, bool regularWindow)
{			
	QFAUIUnit* unitUnderFocus = nullptr;
	QFAUIScroll* scrollUnit = nullptr;
	FindUnitUnderFocus(root, unitUnderFocus, scrollUnit, mousePosX, mousePosY, regularWindow);

	ScrollEvent(root, scrollUnit, delta);
	FocusEvent(unitUnderFocus);
	MouseButtonEvent(unitUnderFocus);

	if (TextInput)
		TextInput->NewFrame(delta);
}

void QFAUIEvent::FindUnitUnderFocus(QFAViewportRoot* root, QFAUIUnit*& unitUnderFocus, QFAUIScroll*& scrollUnit, float mousePosX, float mousePosY, bool regularWindow)
{
	if (root)
	{

		//regularWindow not forget





		SortUIs(root);
		for (int i = SortUIUnits.Length() - 1; i >= 0 ; i--)
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
					unitUnderFocus = SortUIUnits[i];

				if (!scrollUnit && SortUIUnits[i]->Type == QFAUIType::Scroll)
					scrollUnit = (QFAUIScroll*)SortUIUnits[i];

				if (unitUnderFocus && scrollUnit)
					break;
			}
		}
	}
}

void QFAUIEvent::ScrollEvent( QFAViewportRoot* root, QFAUIScroll* scrollUnit, double delta)
{
	if (scrollUnit && scrollUnit->Type == QFAUIType::Type::Scroll)
		QFAUIScroll::NewFrame(scrollUnit, delta, root ? ScrollValue : 0.0f, (LeftShiftDown || RightShiftDown ? true : false));
	else
		QFAUIScroll::NewFrame(nullptr, delta, 0.0f);

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
							last = FocusUnit;
							while (true)
							{
								if (last == curent)
									break;

								last->NotifyOutFocus(true);
								last = last->Parent;
							}

							newUnitUnderFocus->NotifyInFocus();
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
	InputFocusEvent(unitUnderFocus);
	/* process mose press and release */	
	if (LeftMouseDown)
	{
		QFAUISelectUnit::WindowLeftMouseDown(Window, unitUnderFocus);
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

	if (ForwardMouseDown)
	{
		if(unitUnderFocus)
			unitUnderFocus->NotifyForwardMouseDown();

		ForwardMouseDown = false;
	}
	
	if (BackwardMouseDown)
	{
		if(unitUnderFocus)
			unitUnderFocus->NotifyBackwardMouseDown();

		BackwardMouseDown = false;
	}
}

void QFAUIEvent::InputFocusEvent(QFAUIUnit* newUnitUnderFocus)
{
	if (LeftMouseDown)
	{
		double x, y;
		Window->GetMousePosition(x, y);
		QFAUITextInput* oldInput = TextInput;
		if (newUnitUnderFocus && newUnitUnderFocus->Type == QFAUIType::TextInput)
		{			
			((QFAUITextInput*)newUnitUnderFocus)->InInputfocus(x, y);
			TextInput = (QFAUITextInput*)newUnitUnderFocus;
		}
		else
			TextInput = nullptr;

		if (oldInput->IsValid() && oldInput != (QFAUITextInput*)newUnitUnderFocus)
			oldInput->OutInputfocus();
	}
}

void QFAUIEvent::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	for (size_t i = 0; i < Events.size(); i++)
	{
		if (Events[i]->glfWindow == window && Events[i]->Window->RegularWindow)
		{
			if (Events[i]->TextInput->IsValid())
				Events[i]->TextInput->AddChar(codepoint);

			for (size_t j = 0; j < Events[i]->Window->WindowChildCallback.size(); j++)
				if (Events[i]->Window->WindowChildCallback[j].callback)
					Events[i]->Window->WindowChildCallback[j].callback(window, codepoint);

			return;
		}
	}
}

void QFAUIEvent::UnitUnderDelete(QFAUIUnit* deadUnit)
{
	for (size_t i = 0; i < Events.size(); i++)
	{
		if(Events[i]->FocusUnit == deadUnit)
		{
			Events[i]->FocusUnit = nullptr;
			return;
		}
		else if (Events[i]->LeftMouseUnit == deadUnit)
		{
			Events[i]->LeftMouseUnit = nullptr;
			return;
		}
		else if (Events[i]->RightMouseUnit == deadUnit)
		{
			Events[i]->RightMouseUnit = nullptr;
			return;
		}
		else if (Events[i]->TextInput == deadUnit)
		{
			Events[i]->TextInput = nullptr;
			return;
		}
	}
}
