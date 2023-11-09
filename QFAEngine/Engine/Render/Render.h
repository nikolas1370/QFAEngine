#pragma once
#include <Tools/Debug/OpenGlStuff.h>
#include <Math/Math.h>
struct GLFWwindow;
class Camera;
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
	static Camera* CurentCamera;

	static QFAFrameBuffer* secondFrameBuffer;

	static void SetWindow(GLFWwindow* window);
	static void DrawMesh(QMeshBaseComponent* mesh);
	static void DrawMeshShadow(QMeshBaseComponent* mesh);
	static void EndFrame(bool blankScreen = false);
	static void SetCamera(Camera* camera);
	static void StartFrame();

	static void Init();
public:

};


