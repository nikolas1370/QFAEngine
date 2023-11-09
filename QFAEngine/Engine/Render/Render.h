#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>
struct GLFWwindow;
class QCameraComponent;
class QFAShaderProgram;
class QMeshBaseComponent;
class QFAOverlord;
class QFAFrameBuffer;

class QFARender
{
	friend QFAOverlord;

	static int countFarame;
	static double acumulateDeltatime;
	static GLFWwindow* Window;
	static glm::mat4 MatrixPerspective;
	static QCameraComponent* CurentCamera;

	static QFAFrameBuffer* secondFrameBuffer;

	static int Width;
	static int Height;
	static bool WindowSizeChanched ;
	static int NewWidth;
	static int NewHeight;

	static void SetWindow(GLFWwindow* window);
	static void DrawMesh(QMeshBaseComponent* mesh);
	static void DrawMeshShadow(QMeshBaseComponent* mesh);
	static void EndFrame(bool blankScreen = false);
	static void SetCamera(QCameraComponent* camera);
	static void StartFrame();

	static void Init(GLFWwindow* window,int width, int height);


public:

};


