#include "pch.h"
#include "Input.h"
#include <iostream>
#include <EngineStuff/VulkanSuff.h>
#include <EngineStuff/Window/EngineWindow.h>
#include <GLFW/glfw3.h>
std::vector<QFAEngineWindow*> QFAInput::WindowList;

QFAArray<QFAInput::Sinput> QFAInput::Inputs;
FVector2D QFAInput::LastMousePosition;

QFAInput::QFAInput()
{
	if (WindowList.size() == 0)
		stopExecute("QFAEngineWindow not init or all window deleted");

	this->Window = WindowList[0]->glfWindow;
	QFAInput::Inputs.Add({ WindowList[0], this});
}

QFAInput::QFAInput(QFAEngineWindow* window)
{
	for (size_t i = 0; i < WindowList.size(); i++)
	{
		if (WindowList[i] == window)
		{
			this->Window = WindowList[i]->glfWindow;
			QFAInput::Inputs.Add({ window, this });
			return;
		}
	}

	stopExecute("window not valid")
}

QFAInput::~QFAInput()
{
	InputValid = false;
	for (size_t i = 0; i < QFAInput::Inputs.Length(); i++)
	{
		if (QFAInput::Inputs[i].Input == this)
		{
			QFAInput::Inputs.RemoveAt(i);
			return;
		}
	}
}

void QFAInput::ChangeWindow(QFAEngineWindow* window)
{
	Window = window->glfWindow;
	for (size_t i = 0; i < QFAInput::Inputs.Length(); i++)
		if (QFAInput::Inputs[i].Input == this)
			QFAInput::Inputs[i].Window = window;
}

void QFAInput::NewFrame(float delta)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (Inputs[i]->BlockInput || Inputs[i].Window->minimized)
			continue;

		for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
		{
			if (Inputs[i]->KeyHoldList[j].Pressed)
			{
				Inputs[i]->KeyHoldList[j].TimeButtonPressed += delta;
				if (Inputs[i]->KeyHoldList[j].TimeButtonPressed >= Inputs[i]->KeyHoldList[j].HoldTime)
				{
					Inputs[i]->KeyHoldList[j].Pressed = false;
					if (Inputs[i]->KeyHoldList[j].Fun)
						Inputs[i]->KeyHoldList[j].Fun(Inputs[i]->KeyHoldList[j].Key);
				}
			}
		}

		for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
		{
			float axisValue = 0.0f;
			for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
				if (Inputs[i]->AxisList[j].Keys[k].Pressed)
					axisValue += Inputs[i]->AxisList[j].Keys[k].AxisValue;
			
			if (Inputs[i]->AxisList[j].Fun)
				Inputs[i]->AxisList[j].Fun(axisValue);			
		}

		for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
		{
			FVector2D axisValue = FVector2D(0.0f);
			for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
				if (Inputs[i]->Axis2DList[j].Keys[k].Pressed)
					axisValue += Inputs[i]->Axis2DList[j].Keys[k].AxisValue;

			if (Inputs[i]->Axis2DList[j].Fun)
				Inputs[i]->Axis2DList[j].Fun(axisValue);
		}

		for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
		{
			FVector axisValue = FVector(0.0f);
			for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
				if (Inputs[i]->Axis3DList[j].Keys[k].Pressed)
					axisValue += Inputs[i]->Axis3DList[j].Keys[k].AxisValue;			
			
			if (Inputs[i]->Axis3DList[j].Fun)
				Inputs[i]->Axis3DList[j].Fun(axisValue);
		}
	}
}

void QFAInput::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.0001 || yoffset < -0.0001)// scroll in y-axis only
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i]->BlockInput || !Inputs[i]->WheelAxis.Active)
				continue;

			if (Inputs[i]->WheelAxis.Fun)
				Inputs[i]->WheelAxis.Fun((float)yoffset);
		}
	}
}

void QFAInput::MouseMove_callback(GLFWwindow* window, float xoffset, float yoffset)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (Inputs[i]->Window != window || Inputs[i]->BlockInput || !Inputs[i]->MouseMove.Active)
			continue;

		if (Inputs[i]->MouseMove.Fun)
			Inputs[i]->MouseMove.Fun(FVector2D(xoffset, yoffset));
	}
	
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	LastMousePosition = FVector2D((float)x - LastMousePosition.X, (float)y - LastMousePosition.Y);
	for (int i = 0; i < Inputs.Length(); i++)
	{		
		if (Inputs[i]->Window != window || Inputs[i]->BlockInput || !Inputs[i]->MouseMoveAxis.Active)
			continue;

		if (Inputs[i]->MouseMoveAxis.Fun)
			Inputs[i]->MouseMoveAxis.Fun(LastMousePosition);
	}

	LastMousePosition = FVector2D((float)x, (float)y);		
}

void QFAInput::WindowCreated(QFAEngineWindow* window)
{	
	WindowList.push_back(window );
	glfwSetKeyCallback(window->glfWindow, [](GLFWwindow* _window, int key, int scancode, int action, int mods)
		{
			QFAInput::ProcessKey(_window, key, scancode, action, mods);
		});

	glfwSetMouseButtonCallback(window->glfWindow, [](GLFWwindow* _window, int button, int action, int mods)
		{
			QFAInput::ProcessKey(_window, button, 0, action, mods);
		});

	glfwSetScrollCallback(window->glfWindow, [](GLFWwindow* _window, double xoffset, double yoffset)
		{
			QFAInput::Scroll_callback(_window, xoffset, yoffset);
		});

	glfwSetCursorPosCallback(window->glfWindow, [](GLFWwindow* _window, double xpos, double ypos)
		{
			QFAInput::MouseMove_callback(_window, (float)xpos, (float)ypos);
		});

}

void QFAInput::WindowClosed(QFAEngineWindow* window)
{
	for (size_t i = 0; i < WindowList.size(); i++)
	{
		if (WindowList[i] == window) 
		{
			WindowList.erase(WindowList.begin() + i);
			for (int j = Inputs.Length() - 1; j >= 0 ; j--)
			{
				if (Inputs[j].Window == window)
				{					
					Inputs[j].Input->InputValid = false;
					Inputs.RemoveAt(j);
				}
			}

			return;
		}
	}
}

void QFAInput::ProcessKey(GLFWwindow* _window, int key, int scancode, int action, int mods)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (Inputs[i].Window->glfWindow == _window && !Inputs[i]->BlockInput && Inputs[i]->Any.Active && Inputs[i]->Any.Fun)
		{
			Inputs[i]->Any.Active = false;
			Inputs[i]->Any.Fun((EKey::Key)key);
		}
	}
	
	if (action == GLFW_PRESS)
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i].Window->glfWindow != _window || Inputs[i]->BlockInput)
				continue;			

			for (int j = 0; j < Inputs[i]->KeyPressList.Length(); j++)
				if (Inputs[i]->KeyPressList[j].Key == (EKey::Key)key && Inputs[i]->KeyPressList[j].Fun) // && check if fun valide
					Inputs[i]->KeyPressList[j].Fun((EKey::Key)key);

			for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
			{
				if (Inputs[i]->KeyHoldList[j].Key == (EKey::Key)key)
				{
					Inputs[i]->KeyHoldList[j].Pressed = true;
					Inputs[i]->KeyHoldList[j].TimeButtonPressed = 0;
				}
			}
			
			for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
					if (Inputs[i]->AxisList[j].Keys[k].Key == key)
						Inputs[i]->AxisList[j].Keys[k].Pressed = true;

			for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis2DList[j].Keys[k].Key == key)
						Inputs[i]->Axis2DList[j].Keys[k].Pressed = true;

			
			for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis3DList[j].Keys[k].Key == key)
						Inputs[i]->Axis3DList[j].Keys[k].Pressed = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i].Window->glfWindow != _window || Inputs[i]->BlockInput)
				continue;

			for (int j = 0; j < Inputs[i]->KeyReleaseList.Length(); j++)
				if (Inputs[i]->KeyReleaseList[j].Key == (EKey::Key)key && Inputs[i]->KeyReleaseList[j].Fun) // && check if fun valide
					Inputs[i]->KeyReleaseList[j].Fun((EKey::Key)key);

			for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
				if (Inputs[i]->KeyHoldList[j].Key == (EKey::Key)key)
					Inputs[i]->KeyHoldList[j].Pressed = false;

			for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
					if (Inputs[i]->AxisList[j].Keys[k].Key == key)
						Inputs[i]->AxisList[j].Keys[k].Pressed = false;

			for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis2DList[j].Keys[k].Key == key)
						Inputs[i]->Axis2DList[j].Keys[k].Pressed = false;

			for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis3DList[j].Keys[k].Key == key)
						Inputs[i]->Axis3DList[j].Keys[k].Pressed = false;
		}
	}
}


void QFAInput::AddKeyPress(EKey::Key key, std::string id, std::function<void(EKey::Key)> fun)
{
	for (int i = 0; i < KeyPressList.Length(); i++)
	{
		if (KeyPressList[i].Id == id)
		{
			KeyPressList[i].Key = key;
			return;
		}
	}

	KeyPressList.Add(SKeyFunction{ fun, key, id});
}

void QFAInput::RemoveKeyPress(std::string id)
{
	for (int i = 0; i < KeyPressList.Length(); i++)
	{
		if (KeyPressList[i].Id == id)
		{
			KeyPressList.RemoveAt(i);
			return;
		}
	}
}

void QFAInput::AddKeyRelease(EKey::Key key, std::string id, std::function<void(EKey::Key)> fun)
{
	for (int i = 0; i < KeyReleaseList.Length(); i++)
	{
		if (KeyReleaseList[i].Id == id)
		{
			KeyReleaseList[i].Key = key;
			return;
		}
	}

	KeyReleaseList.Add(SKeyFunction{ fun, key, id });
}

void QFAInput::RemoveKeyRelease(std::string Id)
{
	for (int i = 0; i < KeyReleaseList.Length(); i++)
	{
		if (KeyReleaseList[i].Id == Id)
		{
			KeyReleaseList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::SetPressedAnyKey(std::function<void(EKey::Key)> fun)
{
	Any.Active = true;
	Any.Fun = fun;
}

void QFAInput::ActivePressedAnyKey()
{
	if(Any.Fun)
		Any.Active = true;
}

void QFAInput::ShutOffPressedAnyKey()
{
	Any.Active = false;
}

void QFAInput::SetWheelAxis(std::function<void(float)> fun)
{
	WheelAxis.Active = true;
	WheelAxis.Fun = fun;
}

void QFAInput::ShutOffWheelAxis()
{
	WheelAxis.Active = false;
}

void QFAInput::SetMouseMove(std::function<void(FVector2D)> fun)
{
	MouseMove.Active = true;
	MouseMove.Fun = fun;
}

void QFAInput::ShutOffMouseMove()
{
	MouseMove.Active = false;
}

void QFAInput::SetMouseMoveAxis(std::function<void(FVector2D)> fun)
{	
	MouseMoveAxis.Active = true;
	MouseMoveAxis.Fun = fun;
}

void QFAInput::ShutOffMouseMoveAxis()
{
	MouseMoveAxis.Active = false;
}

void QFAInput::AddKeyHold(EKey::Key key, std::string id, float holdTime, std::function<void(EKey::Key)> fun)
{
	for (int i = 0; i < KeyHoldList.Length(); i++)
	{
		if (KeyHoldList[i].Id == id)
		{
			KeyHoldList[i].Key = key;
			return;
		}
	}
	
	KeyHoldList.Add(SKeyHold{ fun, key, id, holdTime});
}


void QFAInput::RemoveKeyHold(std::string id)
{
	for (int i = 0; i < KeyHoldList.Length(); i++)
	{
		if (KeyHoldList[i].Id == id)
		{
			KeyHoldList.RemoveAt(i);
			break;
		}
	}
}

QFAInputAxis1D QFAInput::CreateAxis1D(std::string id, std::function<void(float)> fun)
{	
	for (int i = 0; i < AxisList.Length(); i++)
		if (AxisList[i].Id == id)
			return QFAInputAxis1D(this, id);
	
	AxisList.Add(SAxis1D{ id, fun });
	return QFAInputAxis1D(this, id);
}

QFAInputAxis2D QFAInput::CreateAxis2D(std::string id, std::function<void(FVector2D)> fun)
{
	for (int i = 0; i < Axis2DList.Length(); i++)
		if (Axis2DList[i].Id == id)
			return QFAInputAxis2D(this, id);

	Axis2DList.Add(SAxis2D{ id, fun });
	return QFAInputAxis2D(this, id);
}

QFAInputAxis3D QFAInput::CreateAxis3D(std::string id, std::function<void(FVector)> fun)
{
	for (int i = 0; i < Axis3DList.Length(); i++)
		if (Axis3DList[i].Id == id)
			return QFAInputAxis3D(this, id);

	Axis3DList.Add(SAxis3D{ id, fun });
	return QFAInputAxis3D(this, id);
}

void QFAInput::RemoveAxis1D(std::string id)
{
	for (int i = 0; i < AxisList.Length(); i++)
	{
		if (AxisList[i].Id == id)
		{
			AxisList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::RemoveAxis2D(std::string id)
{
	for (int i = 0; i < Axis2DList.Length(); i++)
	{
		if (Axis2DList[i].Id == id)
		{
			Axis2DList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::RemoveAxis3D(std::string id)
{
	for (int i = 0; i < Axis3DList.Length(); i++)
	{
		if (Axis3DList[i].Id == id)
		{
			Axis3DList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::AddKeyToAxis1D(QFAInput* input, std::string axisId, EKey::Key key, float axisValue, std::string keyId)
{
	for (int j = 0; j < input->AxisList.Length(); j++)
	{
		if (input->AxisList[j].Id == axisId)
		{
			for (int k = 0; k < input->AxisList[j].Keys.Length(); k++)
			{
				if (input->AxisList[j].Keys[k].Id == keyId)
				{
					input->AxisList[j].Keys[k].Key = key;
					return;
				}
			}

			return input->AxisList[j].Keys.Add(SKeyAxis1D{ key, keyId, axisValue });
		}
	}
}

void QFAInput::AddKeyToAxis2D(QFAInput* input, std::string axisId, EKey::Key key, FVector2D axisValue, std::string keyId)
{
	for (int j = 0; j < input->Axis2DList.Length(); j++)
	{
		if (input->Axis2DList[j].Id == axisId)
		{
			for (int k = 0; k < input->Axis2DList[j].Keys.Length(); k++)
			{
				if (input->Axis2DList[j].Keys[k].Id == keyId)
				{
					input->Axis2DList[j].Keys[k].Key = key;
					return;
				}
			}
			
			return input->Axis2DList[j].Keys.Add(SKeyAxis2D{ key, keyId, axisValue });
		}
	}
}

void QFAInput::AddKeyToAxis3D(QFAInput* input, std::string axisId, EKey::Key key, FVector axisValue, std::string keyId)
{
	for (int j = 0; j < input->Axis3DList.Length(); j++)
	{
		if (input->Axis3DList[j].Id == axisId)
		{
			for (int k = 0; k < input->Axis3DList[j].Keys.Length(); k++)
			{
				if (input->Axis3DList[j].Keys[k].Id == keyId)
				{
					input->Axis3DList[j].Keys[k].Key = key;
					return;
				}
			}

			return input->Axis3DList[j].Keys.Add(SKeyAxis3D{ key, keyId, axisValue });
		}
	}
}

void QFAInput::RemoveKeyFromAxis1D(QFAInput* input, std::string axisId, std::string keyId)
{
		for (int j = 0; j < input->AxisList.Length(); j++)
			if (input->AxisList[j].Id == axisId)
				for (int k = 0; k < input->AxisList[j].Keys.Length(); k++)
					if (input->AxisList[j].Keys[k].Id == keyId)
						return input->AxisList[j].Keys.RemoveAt(k);
}

void QFAInput::RemoveKeyFromAxis2D(QFAInput* input, std::string axisId, std::string keyId)
{
	for (int j = 0; j < input->Axis2DList.Length(); j++)
		if (input->Axis2DList[j].Id == axisId)
			for (int k = 0; k < input->Axis2DList[j].Keys.Length(); k++)
				if (input->Axis2DList[j].Keys[k].Id == keyId)
					return input->Axis2DList[j].Keys.RemoveAt(k);
}

void QFAInput::RemoveKeyFromAxis3D(QFAInput* input, std::string axisId, std::string keyId)
{
	for (int j = 0; j < input->Axis3DList.Length(); j++)
		if (input->Axis3DList[j].Id == axisId)
			for (int k = 0; k < input->Axis3DList[j].Keys.Length(); k++)
				if (input->Axis3DList[j].Keys[k].Id == keyId)
					return input->Axis3DList[j].Keys.RemoveAt(k);
}
