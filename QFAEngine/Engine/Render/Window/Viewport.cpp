#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/VulkanSuff.h>
#include <Render/UI/Text.h>
#include <Render/UI/UIParentComponent.h>



void QFAViewport::AddUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;

	if (unit->Parent)
		unit->Parent->RemoveUnitWithoutNotify(unit);
	else if(unit->ParentViewport)
		unit->ParentViewport->RemoveUnitWithoutNotify(unit);
	else
		unit->ParentAttach();

	unit->ParentViewport = this;
	UIUnits.Add(unit);
	if (unit->SelfResizable)
		unit->ViewportSizeChange(Width, Height);
}

void QFAViewport::RemoveUnit(QFAUIUnit* unit)
{
	if (!unit)
		return;
	
	RemoveUnitWithoutNotify(unit);
	unit->ParentDisconect();
}

void QFAViewport::RemoveUnitWithoutNotify(QFAUIUnit* unit)
{
	UIUnits.Remove(unit);
	unit->ParentViewport = nullptr;
}

void QFAViewport::WindowAddMe(QFAWindow* window)
{
	Window = window;
	for (size_t i = 0; i < UIUnits.Length(); i++)
		UIUnits[i]->ParentEnable();
}

void QFAViewport::WindowRemoveMe()
{
	Window = nullptr;
	for (size_t i = 0; i < UIUnits.Length(); i++)
		UIUnits[i]->ParentDisable();
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

	for (size_t i = 0; i < UIUnits.Length(); i++)
		if (UIUnits[i]->SelfResizable)
			UIUnits[i]->ViewportSizeChange(Width, Height);
}



QFAViewport::QFAViewport()
{

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
		
		for (size_t i = 0; i < UIUnits.Length(); i++)
			UIUnits[i]->ParentEnable();
	}
}

void QFAViewport::DeactivateCamera()
{
	for (size_t i = 0; i < UIUnits.Length(); i++)
		UIUnits[i]->ParentDisable();
}

void QFAViewport::CameraChangeParameter(int param)
{	
	if (param == 1)
	{  
		for (size_t i = 0; i < UIUnits.Length(); i++)
			UIUnits[i]->ParentEnable();
	}
	else if(param == 2)
		for (size_t i = 0; i < UIUnits.Length(); i++)
			UIUnits[i]->ParentDisable();
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
			for (size_t i = 0; i < UIUnits.Length(); i++)
				UIUnits[i]->ParentEnable();
		}
		else
			for (size_t i = 0; i < UIUnits.Length(); i++)
				UIUnits[i]->ParentDisable();
	}
	else
	{
		for (size_t i = 0; i < UIUnits.Length(); i++)
			UIUnits[i]->ParentDisable();
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
	
	for (size_t i = 0; i < UIUnits.Length(); i++)
		if (UIUnits[i]->SelfResizable)
			UIUnits[i]->ViewportSizeChange(Width, Height);
}