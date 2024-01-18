#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Text.h>


QFAViewport* QFAViewport::DefaultViewPort = nullptr;

void QFAViewport::AddText(QFAText* text)
{
	if (!text)
		return;

	Texts.Add(text);

}

void QFAViewport::RemoveText(QFAText* text)
{
	Texts.Remove(text);
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

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, 0.0f, 1.0f);

	if (CurentCamera)
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov), (float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	else
		std::cout << "QFAViewport::Settup Camera not set\n";
}



QFAViewport::QFAViewport()
{
	if (!QFAViewport::DefaultViewPort)		
	{
		QFAViewport::DefaultViewPort = this;	
	}	
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

void QFAViewport::ChangeCamera(QCameraComponent* camera)
{
	if (CurentCamera->IsValid())
		CurentCamera->Viewport = nullptr;

	CurentCamera = camera;
	if (CurentCamera->IsValid())
	{
		CurentCamera->IsActive = true;
		CurentCamera->Viewport = this;		
		MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
		//MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
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
	if (Height == 0)
		Height = 1;

	UIProjection = glm::orthoLH_ZO(0.0f, (float)Width, 0.0f, (float)Height, 0.0f, 1.0f);
	MatrixPerspective = glm::perspectiveLH_ZO(glm::radians(CurentCamera->Fov),
		(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); 
}