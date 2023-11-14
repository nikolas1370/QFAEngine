#include "Viewport.h"
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Math.h>
#include <Render/Buffer/Framebuffer.h>
#include <Object/World/World.h>
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Tools/Debug/OpenGlStuff.h>

QFAViewport* QFAViewport::DefaultViewPort = nullptr;

void QFAViewport::Settup(int x, int y, int width, int height)
{
	X = x;
	Y = y;
	Width = width;
	Height = height;
	if (CurentCamera)
		MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
			(float)Height / (float)Width, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f		
	else
		std::cout << "QFAViewport::Settup Camera not set\n";
	
	if (!secondFrameBuffer)
		secondFrameBuffer = new QFAFrameBuffer(width, height);
	else
		secondFrameBuffer->UpdateSize(width, height);
}

void QFAViewport::ProcessFrame()
{
	if (!IsActive || !CurentCamera->IsValid() || !CurentCamera->IsActive)
		return secondFrameBuffer->StartFrame();// blank viewport

	
	CurentFrameWorld = CurentCamera->GetWorld();;
	if (!CurentFrameWorld->IsValid() || !CurentFrameWorld->IsActive)
		return secondFrameBuffer->StartFrame();;// blank viewport

	QDirectionLight* DL = CurentFrameWorld->GetDirectionDight();
	
	DL->StartFrame();
	
	if (DL->GetCastShadow())
		for (int i = 0; i < CurentFrameWorld->Actors.Length(); i++)
			if (CurentFrameWorld->Actors[i]->RootComponent->IsValid())
				ProcessComponentShadow(CurentFrameWorld->Actors[i]->RootComponent);

	secondFrameBuffer->StartFrame();

	for (int i = 0; i < CurentFrameWorld->Actors.Length(); i++)
		if (CurentFrameWorld->Actors[i]->RootComponent->IsValid())
			ProcessComponent(CurentFrameWorld->Actors[i]->RootComponent);
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
	mesh->Bind();
	// now MatrixPerspective send each drawCall in future be change when camera or window change

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
	mesh->Bind(true);
	QFAShaderProgram* shaderProgram = mesh->GetShadowShaderProgram();
	CurentFrameWorld->GetDirectionDight()->SetLightMatrix(CurentCamera->WorldPosition, shaderProgram);
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
}



QFAViewport::QFAViewport()
{
	if (!QFAViewport::DefaultViewPort)
		QFAViewport::DefaultViewPort = this;
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
			(float)Height / (float)Width, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	}
}
