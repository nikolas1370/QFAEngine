#include "Render.h"
#include <Render/Shader/ShaderProgram.h>
#include <iostream>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Render/Time.h>
#include <Object/World/World.h>
#include <Overlord/Overlord.h>
#include "Buffer/FramebufferMain.h"
#include "Buffer/Framebuffer.h"

int QFARender::countFarame = 0;
double QFARender::acumulateDeltatime = 0.0;

GLFWwindow* QFARender::Window;
glm::mat4 QFARender::MatrixPerspective;
QCameraComponent* QFARender::CurentCamera;
QFAFrameBuffer* QFARender::secondFrameBuffer;

int QFARender::Width = 1;
int QFARender::Height = 1;
bool QFARender::WindowSizeChanched = false;
int QFARender::NewWidth;
int QFARender::NewHeight;

void QFARender::SetWindow(GLFWwindow* window)
{
	Window = window;
}

void QFARender::Init(GLFWwindow* window, int width, int height)
{
	Window = window;
	Width = width;
	Height = height;
	QFAFrameBufferMain::Init(width, height);
	secondFrameBuffer = new QFAFrameBuffer(width, height);
	glfwSwapInterval(1);	// 0 vsync off, 1 full vsync, 2 half vsync
	// GLFWwindow* window, int width, int height

	glfwSetWindowSizeCallback(Window, [](GLFWwindow* win, int w, int h)
		{
			/* check if 0*/
			QFARender::NewHeight = h;
			QFARender::NewWidth = w;
			QFARender::WindowSizeChanched = true;
		});	
}

void QFARender::StartFrame()
{
	glViewport(0, 0, Width, Height);
	if (WindowSizeChanched)
	{
		Width = NewWidth;
		Height = NewHeight;
		
		WindowSizeChanched = false;
		secondFrameBuffer->StartFrame(true, Width, Height);
		MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov),
			(float) Width/ (float)Height, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	}
	else
		secondFrameBuffer->StartFrame();
	
}

void QFARender::DrawMesh(QMeshBaseComponent* mesh)
{
	QFAShaderProgram* shaderProgram = mesh->GetShaderProgram();
	mesh->Bind();	
	// now MatrixPerspective send each drawCall in future be change when camera or window change
	
	shaderProgram->SetProjectionMatrix(MatrixPerspective);
	shaderProgram->SetCameraRotationMatrix(CurentCamera->cameraRotationMatrex);
	shaderProgram->SetCameraPosition(FVector(CurentCamera->WorldPosition.X, CurentCamera->WorldPosition.Y, CurentCamera->WorldPosition.Z));
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);
	
	QDirectionLight* dl = QFAOverlord::GetCurentWorld()->GetDirectionDight();
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

void QFARender::DrawMeshShadow(QMeshBaseComponent* mesh)
{
	mesh->Bind(true);
	QFAShaderProgram* shaderProgram = mesh->GetShadowShaderProgram();
	QFAOverlord::GetCurentWorld()->GetDirectionDight()->SetLightMatrix(CurentCamera->WorldPosition, shaderProgram);
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
}

void QFARender::EndFrame(bool blankScreen)
{
	if (blankScreen)
	{
		QFAFrameBufferMain::BlankScreen(Width, Height);
		glfwSwapBuffers(Window);
		return;
	}

	QFAFrameBufferMain::CopyFrameBuffer(secondFrameBuffer, Width, Height);
	glfwSwapBuffers(Window);
	countFarame++;
	acumulateDeltatime += QTime::GetDeltaTime();
	if (acumulateDeltatime >= 1.0)
	{
		countFarame = 0;
		acumulateDeltatime = 0.0;
	}
}

void QFARender::SetCamera(QCameraComponent* camera)
{	
	CurentCamera = camera;
	MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov), 
		(float)Height / (float)Width, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
}