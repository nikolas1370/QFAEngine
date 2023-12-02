#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>

#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Text/Text.h>


QFAViewport* QFAViewport::DefaultViewPort = nullptr;

void QFAViewport::AddText(QFAText* text)
{
	if (!text)
		return;

	Texts.Add(text);
	text->ChangeViewportSize((unsigned int)Width, (unsigned int)Height);
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
	
	UIProjection = glm::ortho(0.0f, (float)Width, 0.0f, (float)Height);

	for (size_t i = 0; i < Texts.Length(); i++)
		Texts[i]->ChangeViewportSize(Width, Height);


	if (CurentCamera)
		MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
			(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	else
		std::cout << "QFAViewport::Settup Camera not set\n";
		
	secondFrameBuffer.UpdateSize(Width, Height);
}

void QFAViewport::ProcessFrame(uint64_t startFrameTime)
{	
	if (!IsActive || !CurentCamera->IsValid() || !CurentCamera->IsActive)
		return secondFrameBuffer.StartFrame();// blank viewport
	
	CurentFrameWorld = CurentCamera->GetWorld();;
	if (!CurentFrameWorld->IsValid() || !CurentFrameWorld->IsActive)
		return secondFrameBuffer.StartFrame();;// blank viewport

	StartFrameTime = startFrameTime;
	QDirectionLight* DL = CurentFrameWorld->GetDirectionDight();
	
	DL->StartFrame();
	
	if (DL->GetCastShadow())
		for (int i = 0; i < CurentFrameWorld->Actors.Length(); i++)
			if (CurentFrameWorld->Actors[i]->RootComponent->IsValid())
				ProcessComponentShadow(CurentFrameWorld->Actors[i]->RootComponent);
	
	secondFrameBuffer.StartFrame();
	
	for (int i = 0; i < CurentFrameWorld->Actors.Length(); i++)
		if (CurentFrameWorld->Actors[i]->RootComponent->IsValid())
			ProcessComponent(CurentFrameWorld->Actors[i]->RootComponent);

	QFAText::StartTextRender(UIProjection);
	for (size_t i = 0; i < Texts.Length(); i++)
		Texts[i]->Render();	

	QFAText::EndTextRender();
}

void QFAViewport::ProcessComponent(QSceneComponent* component)
{
	if (!component->IsValid())
		return;

	if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
		DrawMesh(mesh);

	for (int i = 0; i < component->ListComponents.Length(); i++)
		if (component->ListComponents[i]->IsValid())
			ProcessComponent(component->ListComponents[i]);	
}

void QFAViewport::ProcessComponentShadow(QSceneComponent* component)
{
	if (!component->IsValid())
		return;

	if (QMeshBaseComponent* mesh = dynamic_cast<QMeshBaseComponent*>(component))
		if (mesh->GetCastShadow())
			DrawMeshShadow(mesh);

	for (int i = 0; i < component->ListComponents.Length(); i++)
		if (component->ListComponents[i]->IsValid())
			ProcessComponent(component->ListComponents[i]);	
}

void QFAViewport::DrawMesh(QMeshBaseComponent* mesh)
{
	QFAShaderProgram* shaderProgram = mesh->GetShaderProgram();
	mesh->Bind(StartFrameTime, false);

	shaderProgram->SetProjectionMatrix(MatrixPerspective);
	shaderProgram->SetCameraRotationMatrix(CurentCamera->cameraRotationMatrex);
	shaderProgram->SetCameraPosition(FVector(CurentCamera->WorldPosition.X, CurentCamera->WorldPosition.Y, CurentCamera->WorldPosition.Z));
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	QDirectionLight* dl = CurentFrameWorld->GetDirectionDight();
	if (dl->CastShadows)
	{
		dl->SetLightMatrix(CurentCamera->WorldPosition, shaderProgram);
		dl->SetShadowMap(shaderProgram);
		shaderProgram->SetShadowOn(true);
	}
	else
		shaderProgram->SetShadowOn(false);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
}

void QFAViewport::DrawMeshShadow(QMeshBaseComponent* mesh)
{
	mesh->Bind(StartFrameTime, true);
	QFAShaderProgram* shaderProgram = mesh->GetShadowShaderProgram();
	CurentFrameWorld->GetDirectionDight()->SetLightMatrix(CurentCamera->WorldPosition, shaderProgram);
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
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
		MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
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

	secondFrameBuffer.UpdateSize(Width, Height);
	UIProjection = glm::ortho(0.0f, (float)Width, 0.0f, (float)Height);
	MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
		(float)Width / (float)Height, 0.1f, CurentCamera->ViewDistance);
}


