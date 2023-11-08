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
	}

}

void QFAInput::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset > 0.0001 || yoffset < -0.0001)// scroll in y-axis only
	{

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
}

void QFAInput::ProcessKey(int key, int scancode, int action, int mods)
{
	for (size_t i = 0; i < Inputs.Length(); i++)
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
		}
	}
}


int QFAInput::AddKeyPress(EKey::Key key, std::function<void(EKey::Key)> fun, std::string str_id )
{
	KeyPressList.Add(SKeyFunction{ fun, key,  EventIdCounter++, str_id});
	return EventIdCounter - 1;
}

void QFAInput::RemoveKeyPress(EKey::Key key)
{
	for (int i = KeyPressList.Length() - 1; i >= 0; i--)
		if(KeyPressList[i].key == (EKey::Key)key)
			KeyPressList.RemoveAt(i);
}

void QFAInput::RemoveKeyPressId(int id)
{
	for (int i = KeyPressList.Length() - 1; i >= 0; i--)
		if (KeyPressList[i].id == id)
			KeyPressList.RemoveAt(i);
}

void QFAInput::RemoveKeyPressStrId(std::string str_id)
{
	if (str_id == "")
		return;

	for (int i = KeyPressList.Length() - 1; i >= 0; i--)
		if (KeyPressList[i].string_id == str_id)
			KeyPressList.RemoveAt(i);
}

int QFAInput::AddKeyRelease(EKey::Key key, std::function<void(EKey::Key)> fun, std::string str_id )
{
	KeyReleaseList.Add(SKeyFunction{ fun, key, EventIdCounter++, str_id });
	return EventIdCounter - 1;
}

void QFAInput::RemoveKeyRelease(EKey::Key key)
{
	for (int i = KeyReleaseList.Length() - 1; i >= 0; i--)
		if (KeyReleaseList[i].key == (EKey::Key)key)
			KeyReleaseList.RemoveAt(i);
}

void QFAInput::RemoveKeyReleaseId(int id)
{
	for (int i = KeyReleaseList.Length() - 1; i >= 0; i--)
		if (KeyReleaseList[i].id == id)
			KeyReleaseList.RemoveAt(i);
}

void QFAInput::RemoveKeyReleaseStrId(std::string str_Id)
{
	if (str_Id == "")
		return;

	for (int i = KeyReleaseList.Length() - 1; i >= 0; i--)
		if (KeyReleaseList[i].string_id == str_Id)
			KeyReleaseList.RemoveAt(i);
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


int QFAInput::AddKeyHold(EKey::Key key, std::function<void(EKey::Key)> fun, std::string str_id, float holdTime)
{
	KeyHoldList.Add(SKeyHold{ fun, key, EventIdCounter++, str_id, holdTime});
	return EventIdCounter - 1;	
}

void QFAInput::RemoveKeyHold(EKey::Key key)
{
	for (int i = KeyHoldList.Length() - 1; i >= 0; i--)
		if (KeyHoldList[i].key == (EKey::Key)key)
			KeyHoldList.RemoveAt(i);
}
void QFAInput::RemoveKeyHoldId(int id)
{
	for (int i = KeyHoldList.Length() - 1; i >= 0; i--)
		if (KeyHoldList[i].id == id)
			KeyHoldList.RemoveAt(i);
}

void QFAInput::RemoveKeyHoldStrId(std::string strId)
{
	if (strId == "")
		return;

	for (int i = KeyHoldList.Length() - 1; i >= 0; i--)
		if (KeyHoldList[i].string_id == strId)
			KeyHoldList.RemoveAt(i);
}