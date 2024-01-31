#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Text.h>
#include <Render/UI/UIParentComponent.h>

const float QFAViewport::MinMaxZIndexUI = 1000.0f;

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

	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	else
		std::cout << "QFAViewport::Settup Camera not set\n";


	Root.SetSizeParent(Width, Height);
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
		
		
		if (Window)
			Root.ParentEnable();
	}
}

void QFAViewport::DeactivateCamera()
{	
	Root.ParentDisable();
}

void QFAViewport::CameraChangeParameter(int param)
{	
	if (param == 1)
		Root.ParentEnable();
	else if(param == 2)		
		Root.ParentDisable();
}

void QFAViewport::ChangeCamera(QCameraComponent* camera)
{
	CurentCamera = camera;
	if (CurentCamera->IsValid())
	{
		CurentCamera->Viewport = this;		
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
			(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);

		if (CurentCamera->IsActive)
		{
			if (Window)
				Root.ParentEnable();
		}
		else
			Root.ParentDisable();
				
	}
	else
		Root.ParentDisable();
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

	if (!CurentCamera)
		return;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, QFAViewport::MinMaxZIndexUI * -1, QFAViewport::MinMaxZIndexUI);
	MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
		(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); 


}