#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>
struct GLFWwindow;
class Camera;
class ShaderProgram;
class QMeshBaseComponent;
/* 
Render must be created before someone else (QStaticMesh or some else)
*/
class Render
{
	int countFarame = 0;
	double acumulateDeltatime = 0.0;
	GLuint color_renderbuffer;
	GLFWwindow* Window;
	glm::mat4 MatrixPerspective;
	Camera* CurentCamera;
	unsigned int framebuffer;

	/* main*/
	ShaderProgram* ProgramMain;
	unsigned int VAOMain;
	GLuint IBOMain;
	GLuint VBOMain; // unique id all in opengl have it

	static float VertexMain[];
	static unsigned int indexsMain[];
	/*----*/

	void InitMainFrameBuffer();
	void InitSecondFrameBuffer();	
	void CopyToMain();
	void DrawDepthTexture(unsigned int depthMapId);

public:

	Render();
	void SetWindow(GLFWwindow* window);	
	void DrawMesh(QMeshBaseComponent* mesh);
	void DrawMeshShadow(QMeshBaseComponent* mesh);
	void EndFrame();
	void SetCamera(Camera* camera);
	void StartFrame(unsigned int _framebuffer = 0);
};


