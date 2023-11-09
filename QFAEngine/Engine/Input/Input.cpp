#include "Input.h"
#include <iostream>

GLFWwindow* QFAInput::Window = nullptr;
QFAArray<QFAInput*> QFAInput::Inputs;

QFAInput::QFAInput()
{
	QFAInput::Inputs.Add(this);
}

QFAInput::~QFAInput()
{
	QFAInput::Inputs.Remove(this);
}

void QFAInput::NewFrame(float delta)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (Inputs[i]->BlockInput)
			continue;

		for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
		{
			if (Inputs[i]->KeyHoldList[j].pressed)
			{
				Inputs[i]->KeyHoldList[j].timeButtonPressed += delta;
				if (Inputs[i]->KeyHoldList[j].timeButtonPressed >= Inputs[i]->KeyHoldList[j].HoldTime)
				{
					Inputs[i]->KeyHoldList[j].pressed = false;
					if (Inputs[i]->KeyHoldList[j].fun)
						Inputs[i]->KeyHoldList[j].fun(Inputs[i]->KeyHoldList[j].key);
				}
			}
		}

		for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
		{
			float axisValue = 0.0f;
			for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
				if (Inputs[i]->AxisList[j].Keys[k].pressed)
					axisValue += Inputs[i]->AxisList[j].Keys[k].AxisValue;
			
			if (Inputs[i]->AxisList[j].fun)
				Inputs[i]->AxisList[j].fun(axisValue);			
		}

		for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
		{
			FVector2D axisValue = FVector2D(0.0f);
			for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
				if (Inputs[i]->Axis2DList[j].Keys[k].pressed)
					axisValue += Inputs[i]->Axis2DList[j].Keys[k].AxisValue;

			if (Inputs[i]->Axis2DList[j].fun)
				Inputs[i]->Axis2DList[j].fun(axisValue);
		}

		for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
		{
			FVector axisValue = FVector(0.0f);
			for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
				if (Inputs[i]->Axis3DList[j].Keys[k].pressed)
					axisValue += Inputs[i]->Axis3DList[j].Keys[k].AxisValue;

			if (Inputs[i]->Axis3DList[j].fun)
				Inputs[i]->Axis3DList[j].fun(axisValue);
		}
	}
}

void QFAInput::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.0001 || yoffset < -0.0001)// scroll in y-axis only
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i]->BlockInput || !Inputs[i]->WheelAxis.active)
				continue;

			if (Inputs[i]->WheelAxis.fun)
				Inputs[i]->WheelAxis.fun((float)yoffset);
		}
	}
}

void QFAInput::MouseMove_callback(GLFWwindow* window, float xoffset, float yoffset)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (Inputs[i]->BlockInput || !Inputs[i]->MouseMove.active)
			continue;

		if (Inputs[i]->MouseMove.fun)
			Inputs[i]->MouseMove.fun(FVector2D(xoffset, yoffset));
	}
}

void QFAInput::Init(GLFWwindow* window)
{
	Window = window;
	glfwSetKeyCallback(window, [ ](GLFWwindow* window, int key, int scancode, int action, int mods)
		{			
			QFAInput::ProcessKey(key, scancode, action, mods);
		});

	glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			QFAInput::ProcessKey(button, 0, action, mods);
		});	
	
	glfwSetScrollCallback(Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			QFAInput::Scroll_callback(window, xoffset, yoffset);
		});

	glfwSetCursorPosCallback(Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			QFAInput::MouseMove_callback(window, (float)xpos, (float)ypos);
		});
}

void QFAInput::ProcessKey(int key, int scancode, int action, int mods)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		if (!Inputs[i]->BlockInput && Inputs[i]->Any.active && Inputs[i]->Any.fun)
		{
			Inputs[i]->Any.active = false;
			Inputs[i]->Any.fun((EKey::Key)key);
		}
	}
	
	if (action == GLFW_PRESS)
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i]->BlockInput)
				continue;

			for (int j = 0; j < Inputs[i]->KeyPressList.Length(); j++)
				if (Inputs[i]->KeyPressList[j].key == (EKey::Key)key && Inputs[i]->KeyPressList[j].fun) // && check if fun valide
					Inputs[i]->KeyPressList[j].fun((EKey::Key)key);

			for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
			{
				if (Inputs[i]->KeyHoldList[j].key == (EKey::Key)key)
				{
					Inputs[i]->KeyHoldList[j].pressed = true;
					Inputs[i]->KeyHoldList[j].timeButtonPressed = 0;
				}
			}
			
			for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
					if (Inputs[i]->AxisList[j].Keys[k].key == key)
						Inputs[i]->AxisList[j].Keys[k].pressed = true;

			for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis2DList[j].Keys[k].key == key)
						Inputs[i]->Axis2DList[j].Keys[k].pressed = true;

			for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis3DList[j].Keys[k].key == key)
						Inputs[i]->Axis3DList[j].Keys[k].pressed = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i]->BlockInput)
				continue;

			for (int j = 0; j < Inputs[i]->KeyReleaseList.Length(); j++)
				if (Inputs[i]->KeyReleaseList[j].key == (EKey::Key)key && Inputs[i]->KeyReleaseList[j].fun) // && check if fun valide
					Inputs[i]->KeyReleaseList[j].fun((EKey::Key)key);

			for (int j = 0; j < Inputs[i]->KeyHoldList.Length(); j++)
				if (Inputs[i]->KeyHoldList[j].key == (EKey::Key)key)
					Inputs[i]->KeyHoldList[j].pressed = false;

			for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
					if (Inputs[i]->AxisList[j].Keys[k].key == key)
						Inputs[i]->AxisList[j].Keys[k].pressed = false;

			for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis2DList[j].Keys[k].key == key)
						Inputs[i]->Axis2DList[j].Keys[k].pressed = false;

			for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis3DList[j].Keys[k].key == key)
						Inputs[i]->Axis3DList[j].Keys[k].pressed = false;
		}
	}
}


void QFAInput::AddKeyPress(EKey::Key key, std::string id, std::function<void(EKey::Key)> fun)
{
	for (int i = 0; i < KeyPressList.Length(); i++)
	{
		if (KeyPressList[i].id == id)
		{
			KeyPressList[i].key = key;
			return;
		}
	}

	KeyPressList.Add(SKeyFunction{ fun, key, id});
}

void QFAInput::RemoveKeyPress(std::string id)
{
	for (int i = 0; i < KeyPressList.Length(); i++)
	{
		if (KeyPressList[i].id == id)
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
		if (KeyReleaseList[i].id == id)
		{
			KeyReleaseList[i].key = key;
			return;
		}
	}

	KeyReleaseList.Add(SKeyFunction{ fun, key, id });
}

void QFAInput::RemoveKeyRelease(std::string Id)
{
	for (int i = 0; i < KeyReleaseList.Length(); i++)
	{
		if (KeyReleaseList[i].id == Id)
		{
			KeyReleaseList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::SetPressedAnyKey(std::function<void(EKey::Key)> fun)
{
	Any.active = true;
	Any.fun = fun;
}

void QFAInput::ShutOffPressedAnyKey()
{
	Any.active = false;
}

void QFAInput::SetWheelAxis(std::function<void(float)> fun)
{
	WheelAxis.active = true;
	WheelAxis.fun = fun;
}

void QFAInput::ShutOffWheelAxis()
{
	WheelAxis.active = false;
}

void QFAInput::SetMouseMove(std::function<void(FVector2D)> fun)
{
	MouseMove.active = true;
	MouseMove.fun = fun;
}

void QFAInput::ShutOffMouseMove()
{
	MouseMove.active = false;
}

void QFAInput::AddKeyHold(EKey::Key key, std::string id, float holdTime, std::function<void(EKey::Key)> fun)
{
	for (int i = 0; i < KeyHoldList.Length(); i++)
	{
		if (KeyHoldList[i].id == id)
		{
			KeyHoldList[i].key = key;
			return;
		}
	}

	KeyHoldList.Add(SKeyHold{ fun, key, id, holdTime});
}


void QFAInput::RemoveKeyHold(std::string id)
{
	for (int i = 0; i < KeyHoldList.Length(); i++)
	{
		if (KeyHoldList[i].id == id)
		{
			KeyHoldList.RemoveAt(i);
			break;
		}
	}
}

QFAInputAxis1D QFAInput::CreateAxis1D(std::string id, std::function<void(float)> fun)
{	
	for (int i = 0; i < AxisList.Length(); i++)
		if (AxisList[i].id == id)
			return QFAInputAxis1D();
	
	AxisList.Add(SAxis1D{ id, fun });
	return QFAInputAxis1D(id);
}

QFAInputAxis2D QFAInput::CreateAxis2D(std::string id, std::function<void(FVector2D)> fun)
{
	for (int i = 0; i < Axis2DList.Length(); i++)
		if (Axis2DList[i].id == id)
			return QFAInputAxis2D();

	Axis2DList.Add(SAxis2D{ id, fun });
	return QFAInputAxis2D(id);
}

QFAInputAxis3D QFAInput::CreateAxis3D(std::string id, std::function<void(FVector)> fun)
{
	for (int i = 0; i < Axis3DList.Length(); i++)
		if (Axis3DList[i].id == id)
			return QFAInputAxis3D();

	Axis3DList.Add(SAxis3D{ id, fun });
	return QFAInputAxis3D(id);
}

void QFAInput::RemoveAxis1D(std::string id)
{
	for (int i = 0; i < AxisList.Length(); i++)
	{
		if (AxisList[i].id == id)
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
		if (Axis2DList[i].id == id)
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
		if (Axis3DList[i].id == id)
		{
			Axis3DList.RemoveAt(i);
			break;
		}
	}
}

void QFAInput::AddKeyToAxis1D(std::string axisId, EKey::Key key, float axisValue, std::string keyId)
{// AxisList
	for (int i = 0; i < Inputs.Length(); i++)
	{
		for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
		{
			if (Inputs[i]->AxisList[j].id == axisId)
			{
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
				{
					if (Inputs[i]->AxisList[j].Keys[k].id == keyId)
					{
						Inputs[i]->AxisList[j].Keys[k].key = key;
						return;
					}
				}

				return Inputs[i]->AxisList[j].Keys.Add(SKeyAxis1D{ key, keyId, axisValue });				
			}
		}
	}
}

void QFAInput::AddKeyToAxis2D(std::string axisId, EKey::Key key, FVector2D axisValue, std::string keyId)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
		{
			if (Inputs[i]->Axis2DList[j].id == axisId)
			{
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
				{
					if (Inputs[i]->Axis2DList[j].Keys[k].id == keyId)
					{
						Inputs[i]->Axis2DList[j].Keys[k].key = key;
						return;
					}
				}

				return Inputs[i]->Axis2DList[j].Keys.Add(SKeyAxis2D{ key, keyId, axisValue });				
			}
		}
	}
}

void QFAInput::AddKeyToAxis3D(std::string axisId, EKey::Key key, FVector axisValue, std::string keyId)
{
	for (int i = 0; i < Inputs.Length(); i++)
	{
		for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
		{
			if (Inputs[i]->Axis3DList[j].id == axisId)
			{
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
				{
					if (Inputs[i]->Axis3DList[j].Keys[k].id == keyId)
					{
						Inputs[i]->Axis3DList[j].Keys[k].key = key;
						return;
					}
				}

				return Inputs[i]->Axis3DList[j].Keys.Add(SKeyAxis3D{ key, keyId, axisValue });
			}
		}
	}
}

void QFAInput::RemoveKeyFromAxis1D(std::string axisId, std::string keyId)
{
	for (int i = 0; i < Inputs.Length(); i++)
		for (int j = 0; j < Inputs[i]->AxisList.Length(); j++)
			if (Inputs[i]->AxisList[j].id == axisId)
				for (int k = 0; k < Inputs[i]->AxisList[j].Keys.Length(); k++)
					if (Inputs[i]->AxisList[j].Keys[k].id == keyId)
						return Inputs[i]->AxisList[j].Keys.RemoveAt(k);
}

void QFAInput::RemoveKeyFromAxis2D(std::string axisId, std::string keyId)
{
	for (int i = 0; i < Inputs.Length(); i++)
		for (int j = 0; j < Inputs[i]->Axis2DList.Length(); j++)
			if (Inputs[i]->Axis2DList[j].id == axisId)
				for (int k = 0; k < Inputs[i]->Axis2DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis2DList[j].Keys[k].id == keyId)
						return Inputs[i]->Axis2DList[j].Keys.RemoveAt(k);
}

void QFAInput::RemoveKeyFromAxis3D(std::string axisId, std::string keyId)
{
	for (int i = 0; i < Inputs.Length(); i++)
		for (int j = 0; j < Inputs[i]->Axis3DList.Length(); j++)
			if (Inputs[i]->Axis3DList[j].id == axisId)
				for (int k = 0; k < Inputs[i]->Axis3DList[j].Keys.Length(); k++)
					if (Inputs[i]->Axis3DList[j].Keys[k].id == keyId)
						return Inputs[i]->Axis3DList[j].Keys.RemoveAt(k);						
}
