#include "pch.h"
#include "EngineViewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <EngineStuff/VulkanSuff.h>
#include <UI/Text.h>
#include <UI/UIParentMultipleUnit.h>
#include <EngineStuff/Window/ViewportHolder.h>

const float QFAEngineViewport::MinMaxZIndexUI = 1000.0f;
bool QFAEngineViewport::InGame = false;


std::vector<QFAVKBuffer*> QFAEngineViewport::MeshVertexBuffers;

QWorld* QFAEngineViewport::GetWorld()
{
	if (CurentCamera->IsValid())
		return CurentCamera->GetWorld();
	else
		return nullptr;
}

FVector2D QFAEngineViewport::GetPosition()
{
#if QFA_EDITOR_ONLY
	if (Window->RegularWindow)
		return FVector2D(X , Y );
	else
		return FVector2D(X + Window->Holder->X, Y + Window->Holder->Y);
#else
	return FVector2D(X , Y );
#endif
}



void QFAEngineViewport::WindowAddMe(QFAWindow* window)
{
	Window = window;	
	Root.ParentEnable();
}

void QFAEngineViewport::WindowRemoveMe()
{
	Window = nullptr;
	
	Root.ParentDisable();
}





QFAEngineViewport::QFAEngineViewport()
{
	Root.Viewport = this;
}

QFAEngineViewport::~QFAEngineViewport()
{
	 
}

inline void QFAEngineViewport::ActivateCamera()
{
	if (CurentCamera->IsValid())
	{
		CurentCamera->IsActive = true;
		CurentCamera->Viewport = this;
	}
}

void QFAEngineViewport::DeactivateCamera()
{	

}

void QFAEngineViewport::CameraChangeParameter(int param)
{	

}

void QFAEngineViewport::ChangeCamera(QCameraComponent* camera)
{
	CurentCamera = camera;
	if (CurentCamera->IsValid())
	{
		CurentCamera->Viewport = this;
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
			(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);
	}
}

std::vector<QFAEngineViewport::AnalyzeResult>& QFAEngineViewport::AnalyzeString(const char* str)
{
	static std::vector<AnalyzeResult> ars;
	ars.clear();
	int offset = 0;

	while(true) 
	{
		if (str[offset] == 0)
			break;

		try
		{
			AnalyzeResult as;
			as.Value = std::stof(std::string((char*)&str[offset]));;
			as.Action = ValueAction::None;
			while (true)
			{
				offset++;
				if (str[offset] == 0)
				{
					if (str[offset - 1] == '%')
						as.Type = StrValueType::Percent;
					else
						as.Type = StrValueType::Pixel;

					break;
				}

				if (str[offset] == ' ')
				{
					if (str[offset - 1] == '%')
						as.Type = StrValueType::Percent;
					else
						as.Type = StrValueType::Pixel;

					offset++;
					break;
				}
			}

			ars.push_back(as);
		}
		catch (const std::exception&) 
		{
			AnalyzeResult as;
			as.Action = ValueAction::None;
			while (true)
			{
				if (str[offset] == 0)
					break;

				switch (str[offset])
				{
					case '+':	as.Action = ValueAction::Add; break;
					case '-':	as.Action = ValueAction::Minus; break;
					case '*':	as.Action = ValueAction::Multiply; break;
					case '/':	as.Action = ValueAction::Division;  break;
				}
				
				if (as.Action != ValueAction::None)
				{
					offset += 2;
					ars.push_back(as);
					break;
				}
				else
					offset++;
			}
		}
	}

	return ars;
}

float QFAEngineViewport::GetValue(const char* str, int percentValue, bool is_Size)
{
	std::vector<QFAEngineViewport::AnalyzeResult>& ars = AnalyzeString(str);
	if (!ars.size())
	{
		if (is_Size)
			return 1;

		return 0;
	}

	bool b = true;
	for (int i = 0; i < ars.size() - 1; i++) // check if string have valide data
	{
		std::cout << i << " " <<  ars[i].Action << " " << ars[i + 1].Value << "\n";
		if (b)// i is value i+1 is Action
		{
			if (!(ars[i].Action == ValueAction::None && ars[i + 1].Action > ValueAction::None))
				stopExecute("")
		}
		else// i is Action i+1 is value
		{
			if (!(ars[i].Action > ValueAction::None && ars[i + 1].Action == ValueAction::None))
				stopExecute("")
		}

		b = !b;
	}

	int offset = 0;
	float value = ars[0].Value;
	if (ars[0].Type == StrValueType::Percent)
		value = (value / 100) * (float)percentValue;

	for (int i = 1; i < ars.size() - 1; i += 2)
	{
		AnalyzeResult& res = ars[i];
		AnalyzeResult& resV = ars[i + 1];
		if (resV.Type == StrValueType::Percent)
			resV.Value = (resV.Value / 100) * (float)percentValue;

		switch (res.Action)
		{
			case QFAEngineViewport::Add:		value += resV.Value; break;
			case QFAEngineViewport::Minus:		value -= resV.Value; break;
			case QFAEngineViewport::Multiply:	value *= resV.Value; break;
			case QFAEngineViewport::Division:	if (abs(resV.Value) > 0.00001)
													value /= resV.Value;

												break;
		}
	}

	return value;
}

void QFAEngineViewport::UpdateX()
{
	if (!LeftChars)
	{
		X = 0;
		return;
	}
	
	X = (int)GetValue(LeftChars, WindowWidth, false);
}

void QFAEngineViewport::UpdateY()
{
	if (!TopChars)
	{
		Y = 0;
		return;
	}

	Y = (int)GetValue(TopChars, WindowHeight, false);
}

void QFAEngineViewport::UpdateHeight()
{
	if (!HeightChars)
	{
		Height = WindowHeight;
		return;
	}

	Height = (int)GetValue(HeightChars, WindowHeight, true);
}

void QFAEngineViewport::UpdateWidth()
{
	if (!WidthtChars)
	{
		Width = WindowWidth;
		return;
	}

	Width = (int)GetValue(WidthtChars, WindowWidth, true);
}

void QFAEngineViewport::SetTop(const char* top)
{
	TopChars = top;
	UpdateY();
}

void QFAEngineViewport::SetLeft(const char* left)
{
	LeftChars = left;
	UpdateX();
}

void QFAEngineViewport::SetHeight(const char* height)
{
	HeightChars = height;
	UpdateHeight();
}

void QFAEngineViewport::SetWidth(const char* width)
{
	WidthtChars = width;
	UpdateWidth();
}

void QFAEngineViewport::SettupInside(int windowWidth, int windowHeight)
{
	WindowWidth = windowWidth;
	WindowHeight = windowHeight;

	UpdateX();
	UpdateY();
	UpdateHeight();
	UpdateWidth();
	if (Height == 0)
		Height = 1;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAEngineViewport::MinMaxZIndexUI * -1, QFAEngineViewport::MinMaxZIndexUI);
	Root.SetSizeParent(Width, Height);
	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
}
