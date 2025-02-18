#include "pch.h"
#include "EngineViewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Core/EngineStuff/VulkanSuff.h>
#include <UI/Text.h>
#include <UI/UIParentMultipleUnit.h>
#include <Core/EngineStuff/Window/ViewportHolder.h>
#include <Tools/String.h>

const float QFAEngineViewport::MinMaxZIndexUI = 1000.0f;
#if QFA_EDITOR_ONLY
bool QFAEngineViewport::InGame = false;
#endif

std::vector<QFAVKBuffer*> QFAEngineViewport::MeshVertexBuffers;

QWorld* QFAEngineViewport::GetWorld()
{
	if (CurentCamera->IsValid())
		return CurentCamera->GetWorld();
	else
		return nullptr;
}

FVector2D QFAEngineViewport::GetPositionRelativeRegularWindow()
{
#if QFA_EDITOR_ONLY
	if (Window->RegularWindow)
		return FVector2D(X, Y);
	else
		return FVector2D(X + Window->Holder->X, Y + Window->Holder->Y);
#else
	return FVector2D(X, Y);
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
	Root.removeAllUnit(false);
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

void QFAEngineViewport::UpdateWidth()
{
	if (!WidthtChars)
	{
		Width = WindowWidth;
		Root.ParentSetWidth = Width;
		Root.SetWidth(Root.StrWidth, false);
		return;
	}

	Width = (int)QFAString::GetValue(WidthtChars, WindowWidth, true);
	Root.ParentSetWidth = Width;
	Root.SetWidth(Root.StrWidth, false);
}

void QFAEngineViewport::UpdateHeight()
{
	if (!HeightChars)
	{
		Height = WindowHeight;
		Root.ParentSetHeight = Height;
		Root.SetHeight(Root.StrHeight, false);
		return;
	}

	Height = (int)QFAString::GetValue(HeightChars, WindowHeight, true);
	Root.ParentSetHeight = Height;
	Root.SetHeight(Root.StrHeight, false);
}

#if QFA_EDITOR_ONLY
	void QFAEngineViewport::SetInGame(bool ingame)
	{
	
		InGame = ingame;
	}

	bool QFAEngineViewport::GetInGame()
	{
		return InGame;
	}
#endif

void QFAEngineViewport::UpdateX()
{
	if (!LeftChars)
	{
		X = 0;
		Root.ParentSetPosition_x = X;
		Root.SetLeft(nullptr);
		return;
	}


	X = (int)QFAString::GetValue(LeftChars, WindowWidth, false);
	Root.ParentSetPosition_x = X;
	Root.SetLeft(nullptr);
}

void QFAEngineViewport::UpdateY()
{
	if (!TopChars)
	{
		Y = 0;
		Root.ParentSetPosition_y = Y;
		Root.SetTop(nullptr);
		return;
	}

	Y = (int)QFAString::GetValue(TopChars, WindowHeight, false);
	Root.ParentSetPosition_y = Y;
	Root.SetTop(nullptr);
}

void QFAEngineViewport::SetWidth(const char* width)
{
	WidthtChars = width;
	UpdateWidth();
}

void QFAEngineViewport::SetHeight(const char* height)
{
	HeightChars = height;
	UpdateHeight();
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



void QFAEngineViewport::SettupInside(int windowWidth, int windowHeight)
{
	WindowWidth = windowWidth;
	WindowHeight = windowHeight;

	UpdateWidth();
	UpdateHeight();	
	UpdateX();
	UpdateY();
	if (Height == 0)
		Height = 1;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAEngineViewport::MinMaxZIndexUI * -1, QFAEngineViewport::MinMaxZIndexUI);	
	Root.ParentSetWidth = Width;
	Root.ParentSetHeight = Height;
	Root.SetWidth(Root.StrWidth, Root.ParentSetWidthMinus);
	Root.SetHeight(Root.StrHeight, Root.ParentSetHeightMinus);

	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
}
