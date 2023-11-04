#include "Render.h"
#include <Render/Shader/ShaderProgram.h>
#include <iostream>
#include <Object/Camera.h>
#include <Object/ActorComponent/SceneComponent/Mesh/MeshBase.h>
#include <Render/Time.h>
#include <Object/World/World.h>


float Render::VertexMain[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f, 
	-1.0f,  1.0f, 0.0f, 1.0f, 
	 1.0f,  1.0f, 1.0f, 1.0f, 
	 1.0f, -1.0f, 1.0f, 0.0f, 
};

unsigned int Render::indexsMain[] =
{
	0,1,2,2,3,0
};

void Render::InitMainFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
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

	ProgramMain = new ShaderProgram("Engine/Shaders/MainFrameBuffer/Vertex.shader", "Engine/Shaders/MainFrameBuffer/fragment.shader");
}

void Render::InitSecondFrameBuffer()
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

void Render::CopyToMain()
{	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// glBlitFramebuffer — copy a block of pixels from the read framebuffer to the draw framebuffer
	glBlitFramebuffer(0, 0, 600, 600, 0, 0, 600, 600, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);	
	glfwSwapBuffers(Window);
}

void Render::DrawDepthTexture(unsigned int depthMapId)
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

Render::Render()
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

void Render::SetWindow(GLFWwindow* window)
{
	Window = window;
}

void Render::StartFrame(unsigned int _framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);	
	glViewport(0, 0, 600, 600);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Render::DrawMesh(QMeshBaseComponent* mesh)
{
	if (!CurentCamera)
	{
		std::cout << "Render::DrawMesh camera not set" << std::endl;
		return;
	}

	ShaderProgram* shaderProgram = mesh->GetShaderProgram();
	mesh->Bind();
	
	// now MatrixPerspective send each drawCall in future be change when camera or window change
	MatrixPerspective = glm::perspective(glm::radians(CurentCamera->Fov), (float)600 / (float)600, 0.1f, CurentCamera->ViewDistance); // (near) not Less than 0.1f
	
	shaderProgram->SetProjectionMatrix(MatrixPerspective);
	shaderProgram->SetCameraRotationMatrix(CurentCamera->cameraRotationMatrex);
	FVector GLCameraPos = CurentCamera->GetOpenGLPosition();
	shaderProgram->SetCameraPosition(FVector(GLCameraPos.X, GLCameraPos.Y, GLCameraPos.Z));
	
	

	glm::mat4 lox(mesh->RotationMatrix);	
	lox[3][0] = mesh->WorldPosition.X;
	lox[3][1] = mesh->WorldPosition.Y;
	lox[3][2] = mesh->WorldPosition.Z;
	lox[3][3] = 1;
	lox = glm::scale(lox, glm::vec3(mesh->AccumulateScale.Y * mesh->Scale.Y, mesh->AccumulateScale.Z * mesh->Scale.Z, mesh->AccumulateScale.X * mesh->Scale.X));
	shaderProgram->SetModelMatrix(lox);	



	QDirectionLight* dl = QWorld::GetCurentWorld()->GetDirectionDight();
	if (dl->CastShadows)
	{		
		dl->SetLightMatrix(CurentCamera->GetOpenGLPosition(), shaderProgram);		
		dl->SetShadowMap(shaderProgram);
		shaderProgram->SetShadowOn(true);
		1 + 1;
	}
	else
		shaderProgram->SetShadowOn(false);

	1 + 1;
	
	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));	
}

void Render::DrawMeshShadow(QMeshBaseComponent* mesh)
{
	mesh->Bind(true);
	ShaderProgram* shaderProgram = mesh->GetShadowShaderProgram();
	QWorld::GetCurentWorld()->GetDirectionDight()->SetLightMatrix(CurentCamera->GetOpenGLPosition(), shaderProgram);		
	shaderProgram->SetModelMatrix(mesh->ModelMatrix);

	GLCall(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr));
}

void Render::EndFrame()
{
	if (!Window)
	{
		std::cout << "Render::EndFrame window not set" << std::endl;
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

void Render::SetCamera(Camera* camera)
{
	CurentCamera = camera;
}

/*
ray/AABB		axys fixation whitout rotatio
bool intersection(box b, ray r) {
	double tx1 = (b.min.x - r.x0.x)*r.n_inv.x;
	double tx2 = (b.max.x - r.x0.x)*r.n_inv.x;

	double tmin = min(tx1, tx2);
	double tmax = max(tx1, tx2);

	double ty1 = (b.min.y - r.x0.y)*r.n_inv.y;
	double ty2 = (b.max.y - r.x0.y)*r.n_inv.y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));

	return tmax >= tmin;
}

// z
float rayAABBIntersection (Ray ray, AABB aabb)
{
	float tx1 = (aabb.min.x - ray.origin.x)*ray.inverseDirection.x;
	float tx2 = (aabb.max.x - ray.origin.x)*ray.inverseDirection.x;

	float tmin = min(tx1, tx2);
	float tmax = max(tx1, tx2);

	float ty1 = (aabb.min.y - ray.origin.y)*ray.inverseDirection.y;
	float ty2 = (aabb.max.y - ray.origin.y)*ray.inverseDirection.y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));

	float tz1 = (aabb.min.z - ray.origin.z)*ray.inverseDirection.z;
	float tz2 = (aabb.max.z - ray.origin.z)*ray.inverseDirection.z;

	tmin = max(tmin, min(tz1, tz2));
	tmax = min(tmax, max(tz1, tz2));

	if (tmin > tmax)
	{
		return INFINITY;
	}

	if (tmin < 0)
	{
		return tmax;
	}

	return tmin;
}




///---- not ray/AABB
//whit rotatio
https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
// r.dir is unit direction vector of ray
dirfrac.x = 1.0f / r.dir.x;
dirfrac.y = 1.0f / r.dir.y;
dirfrac.z = 1.0f / r.dir.z;
// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
// r.org is origin of ray
float t1 = (lb.x - r.org.x)*dirfrac.x;
float t2 = (rt.x - r.org.x)*dirfrac.x;
float t3 = (lb.y - r.org.y)*dirfrac.y;
float t4 = (rt.y - r.org.y)*dirfrac.y;
float t5 = (lb.z - r.org.z)*dirfrac.z;
float t6 = (rt.z - r.org.z)*dirfrac.z;

float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
if (tmax < 0)
{
	t = tmax;
	return false;
}

// if tmin > tmax, ray doesn't intersect AABB
if (tmin > tmax)
{
	t = tmax;
	return false;
}

//t = tmin;  whit out in box check
// whit check
//I have added to @zacharmarz answer to handle when the ray origin is inside of the AABB. In this case tmin will be negative and behind the ray so tmax is the first intersection between the ray and AABB.
// if tmin < 0 then the ray origin is inside of the AABB and tmin is behind the start of the ray so tmax is the first intersection
if(tmin < 0) {
  t = tmax;
} else {
  t = tmin;
}


return true;
*/


