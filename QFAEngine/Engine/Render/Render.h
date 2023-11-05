#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>
struct GLFWwindow;
class Camera;
class QFAShaderProgram;
class QMeshBaseComponent;
class QFAOverlord;

class QFARender
{
	friend QFAOverlord;

	static int countFarame;
	static double acumulateDeltatime;
	static GLuint color_renderbuffer;
	static GLFWwindow* Window;
	static glm::mat4 MatrixPerspective;
	static Camera* CurentCamera;
	static unsigned int framebuffer;

	/* main*/
	static QFAShaderProgram* ProgramMain;
	static unsigned int VAOMain;
	static GLuint IBOMain;
	static GLuint VBOMain;

	static float VertexMain[];
	static unsigned int indexsMain[];
	/*----*/

	static void InitMainFrameBuffer();
	static void InitSecondFrameBuffer();	
	static void CopyToMain();
	static void DrawDepthTexture(unsigned int depthMapId);

	static void SetWindow(GLFWwindow* window);
	static void DrawMesh(QMeshBaseComponent* mesh);
	static void DrawMeshShadow(QMeshBaseComponent* mesh);
	static void EndFrame(bool blankScreen = false);
	static void SetCamera(Camera* camera);
	static void StartFrame(unsigned int _framebuffer = 0);

	static void Init();
public:

};


