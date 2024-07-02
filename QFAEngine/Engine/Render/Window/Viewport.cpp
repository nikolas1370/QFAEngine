#include "pch.h"
#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Text.h>
#include <Render/UI/UIParentMultipleUnit.h>

const float QFAViewport::MinMaxZIndexUI = 1000.0f;

std::vector<QFAVKBuffer*> QFAViewport::MeshVertexBuffers;

QWorld* QFAViewport::GetWorld()
{
	if (CurentCamera->IsValid())
		return CurentCamera->GetWorld();
	else
		return nullptr;
}

void QFAViewport::WindowAddMe(QFAWindow* window)
{
	Window = window;	
	Root.ParentEnable();
}

void QFAViewport::WindowRemoveMe()
{
	Window = nullptr;
	
	Root.ParentDisable();
}


void QFAViewport::Settup(int windowWidth, int windowHeight)
{
	WindowWidth = windowWidth;
	WindowHeight = windowHeight;
	X = (int)((float)WindowWidth * XP);
	Y = (int)((float)windowHeight * YP);
	Width = (int)((float)WindowWidth * WidthP);
	Height = (int)((float)WindowHeight * HeightP);
	if (Height == 0)
		Height = 1;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAViewport::MinMaxZIndexUI * -1, QFAViewport::MinMaxZIndexUI);
	Root.SetSizeParent(Width, Height);
	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	



	
}



QFAViewport::QFAViewport()
{
	Root.Viewport = this;
}

QFAViewport::~QFAViewport()
{
	 
}

inline void QFAViewport::ActivateCamera()
{
	if (CurentCamera->IsValid())
	{
		CurentCamera->IsActive = true;
		CurentCamera->Viewport = this;
	}
}

void QFAViewport::DeactivateCamera()
{	

}

void QFAViewport::CameraChangeParameter(int param)
{	

}

void QFAViewport::ChangeCamera(QCameraComponent* camera)
{
	CurentCamera = camera;
	if (CurentCamera->IsValid())
	{
		CurentCamera->Viewport = this;
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
			(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);
	}


}

void QFAViewport::SetParameters(float xP, float  yP, float widthP, float heightP)
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
	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAViewport::MinMaxZIndexUI * -1, QFAViewport::MinMaxZIndexUI);
	if (!CurentCamera)
		return;
	
	MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
		(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);
}