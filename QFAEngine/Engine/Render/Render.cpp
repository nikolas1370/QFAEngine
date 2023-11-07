#include "Render.h"
#include <Render/Shader/ShaderProgram.h>
#include <iostream>
#include <Object/Camera.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Render/Time.h>
#include <Object/World/World.h>
#include <Overlord/Overlord.h>

int QFARender::countFarame = 0;
double QFARender::acumulateDeltatime = 0.0;
GLuint QFARender::color_renderbuffer;
GLFWwindow* QFARender::Window;
glm::mat4 QFARender::MatrixPerspective;
Camera* QFARender::CurentCamera;
unsigned int QFARender::framebuffer;

QFAShaderProgram* QFARender::ProgramMain;
unsigned int QFARender::VAOMain = 0;
GLuint QFARender::IBOMain;
GLuint QFARender::VBOMain; // unique id all in opengl have it

float QFARender::VertexMain[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f, 
	-1.0f,  1.0f, 0.0f, 1.0f, 
	 1.0f,  1.0f, 1.0f, 1.0f, 
	 1.0f, -1.0f, 1.0f, 0.0f, 
};

unsigned int QFARender::indexsMain[] =
{
	0,1,2,2,3,0
};

void QFARender::InitMainFrameBuffer()
{	
	GLCall(glGenVertexArrays(1, &VAOMain));
	GLCall(glBindVertexArray(VAOMain));
	
	GLCall(glGenBuffers(1, &VBOMain));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBOMain));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexMain), VertexMain, GL_STATIC_DRAW));

	glEnableVertexAttribArray(0);
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0));
	glEnableVertexAttribArray(1);
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)8));

	glGenBuffers(1, &IBOMain); // gen index buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOMain));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexsMain), indexsMain, GL_STATIC_DRAW));

	ProgramMain = new QFAShaderProgram("Engine/Shaders/MainFrameBuffer/Vertex.shader", "Engine/Shaders/MainFrameBuffer/fragment.shader");
}

void QFARender::InitSecondFrameBuffer()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);//GL_FRAMEBUFFER read and write (GL_DRAW_FRAMEBUFFER only write)

	/*---------  i Gen Renderbuffers for color*/
	glGenRenderbuffers(1, &color_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, color_renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, 600, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	unsigned int rbo;// attach DEPTH STENCIL buffer
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	/*---  attach color_renderbuffer*/
	// in EndFrame copy from second frame bufer to  defauld frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer); 

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "!--------------------------------" << std::endl;
}

void QFARender::CopyToMain()
{	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// glBlitFramebuffer — copy a block of pixels from the read framebuffer to the draw framebuffer
	glBlitFramebuffer(0, 0, 600, 600, 0, 0, 600, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);	
	glfwSwapBuffers(Window);
}

void QFARender::DrawDepthTexture(unsigned int depthMapId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	GLCall(glBindVertexArray(VAOMain));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProgramMain->Use();
	GLCall(GLint location = glGetUniformLocation(ProgramMain->ShaderProgramId, "u_Texture"));
	if (location == -1)
	{
		std::cout << "warnong uniform no exist: " << std::endl;
		ASSERT(false);
		return;
	}

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, depthMapId)); // bind texture to GL_TEXTURE0

	GLCall(glUniform1i(location, 0));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
	glfwSwapBuffers(Window);
}



void QFARender::SetWindow(GLFWwindow* window)
{
	Window = window;
}

void QFARender::StartFrame(unsigned int _framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);	
	glViewport(0, 0, 600, 600);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void QFARender::Init()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));// settup how opengl by with alpha
	GLCall(glEnable(GL_BLEND));

	InitMainFrameBuffer();
	InitSecondFrameBuffer();
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1);
		// glBlitFramebuffer — copy a block of pixels from the read framebuffer to the draw framebuffer
		glBlitFramebuffer(0, 0, 600, 600, 0, 0, 600, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glfwSwapBuffers(Window);
		return;
	}

	CopyToMain();
	countFarame++;
	acumulateDeltatime += QTime::GetDeltaTime();
	if (acumulateDeltatime >= 1.0)
	{
		
		//printf("latency %f, frame %d \n", acumulateDeltatime / (float)countFarame * 1000.0, countFarame);
		countFarame = 0;
		acumulateDeltatime = 0.0;
	}
}

void QFARender::SetCamera(Camera* camera)
{
	CurentCamera = camera;
}



