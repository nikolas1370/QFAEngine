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
	if (action == GLFW_PRESS)
	{
		for (int i = 0; i < Inputs.Length(); i++)
		{
			if (Inputs[i]->BlockInput)
				continue;

			for (int j = 0; j < Inputs[i]->KeyPressList.Length(); j++)
				if (Inputs[i]->KeyPressList[j].key == (EKey::Key)key && Inputs[i]->KeyPressList[j].fun) // && check if fun valide
					Inputs[i]->KeyPressList[j].fun((EKey::Key)key);
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
		}
	}
}


void QFAInput::AddKeyPress(EKey::Key key, std::function<void(EKey::Key)> fun)
{
	KeyPressList.Add(SKeyFunction{ fun, key });
}

void QFAInput::RemoveKeyPress(EKey::Key key)
{
	for (int i = KeyPressList.Length() - 1; i >= 0; i--)
		if(KeyPressList[i].key == (EKey::Key)key)
			KeyPressList.RemoveAt(i);
}

void QFAInput::AddKeyRelease(EKey::Key key, std::function<void(EKey::Key)> fun)
{
	KeyReleaseList.Add(SKeyFunction{ fun, key });
}

void QFAInput::RemoveKeyRelease(EKey::Key key)
{
	for (int i = KeyReleaseList.Length() - 1; i >= 0; i--)
		if (KeyReleaseList[i].key == (EKey::Key)key)
			KeyReleaseList.RemoveAt(i);
}
