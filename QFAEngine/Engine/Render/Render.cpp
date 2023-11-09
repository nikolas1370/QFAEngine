#include "Render.h"
#include <Render/Shader/ShaderProgram.h>
#include <iostream>
#include <Object/Camera.h>
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
Camera* QFARender::CurentCamera;
QFAFrameBuffer* QFARender::secondFrameBuffer;

void QFARender::SetWindow(GLFWwindow* window)
{
	Window = window;
}

void QFARender::StartFrame()
{
	secondFrameBuffer->StartFrame();
}

void QFARender::Init()
{
	QFAFrameBufferMain::Init();
	secondFrameBuffer = new QFAFrameBuffer;
	glfwSwapInterval(1);	// 0 vsync off, 1 full vsync, 2 half vsync
}

void QFARender::DrawMesh(QMeshBaseComponent* mesh)
{
	QFAShaderProgram* shaderProgram = mesh->GetShaderProgram();
	mesh->Bind();	
	// now MatrixPerspective send each drawCall in future be change when camera or window change
	MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov), (float)600 / (float)600, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f	
	shaderProgram->SetProjectionMatrix(MatrixPerspective);
	shaderProgram->SetCameraRotationMatrix(CurentCamera->cameraRotationMatrex);
	FVector GLCameraPos = CurentCamera->GetOpenGLPosition();
	shaderProgram->SetCameraPosition(FVector(GLCameraPos.X, GLCameraPos.Y, GLCameraPos.Z));
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);
	
	QDirectionLight* dl = QFAOverlord::GetCurentWorld()->GetDirectionDight();
	if (dl->CastShadows)
	{		
		dl->SetLightMatrix(CurentCamera->GetOpenGLPosition(), shaderProgram);		
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
	QFAOverlord::GetCurentWorld()->GetDirectionDight()->SetLightMatrix(CurentCamera->GetOpenGLPosition(), shaderProgram);		
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
}

void QFARender::EndFrame(bool blankScreen)
{
	if (blankScreen)
	{
		QFAFrameBufferMain::BlankScreen();
		glfwSwapBuffers(Window);
		return;
	}

	QFAFrameBufferMain::CopyFrameBuffer(secondFrameBuffer);
	glfwSwapBuffers(Window);
	countFarame++;
	acumulateDeltatime += QTime::GetDeltaTime();
	if (acumulateDeltatime >= 1.0)
	{
		countFarame = 0;
		acumulateDeltatime = 0.0;
	}
}

void QFARender::SetCamera(Camera* camera)
{
	CurentCamera = camera;
}