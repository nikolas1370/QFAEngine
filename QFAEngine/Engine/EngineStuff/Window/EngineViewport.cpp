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

void QFAEngineViewport::SettupInside(int windowWidth, int windowHeight)
{
	WindowWidth = windowWidth;
	WindowHeight = windowHeight;
	X = (int)((float)WindowWidth * XP);
	Y = (int)((float)windowHeight * YP);
	Width = (int)((float)WindowWidth * WidthP);
	Height = (int)((float)WindowHeight * HeightP);
	if (Height == 0)
		Height = 1;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAEngineViewport::MinMaxZIndexUI * -1, QFAEngineViewport::MinMaxZIndexUI);
	Root.SetSizeParent(Width, Height);
	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
}

void QFAEngineViewport::SetParameters(float xP, float  yP, float widthP, float heightP)
{
	XP = xP;
	YP = yP;
	WidthP = widthP;
	HeightP = heightP;
	X = (int)((float)WindowWidth * XP);
	Y = (int)((float)WindowHeight * YP);
	Width = (int)((float)WindowWidth * WidthP);
	Height = (int)((float)WindowHeight * HeightP);
	Root.SetSizeParent(Width, Height);	
	if (Height == 0)
		Height = 1;

	// need change if camera not set
	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAEngineViewport::MinMaxZIndexUI * -1, QFAEngineViewport::MinMaxZIndexUI);
	if (!CurentCamera)
		return;
	
	MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
		(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);
}
