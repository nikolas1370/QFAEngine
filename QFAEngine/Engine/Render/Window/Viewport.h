#pragma once
#include <Math/Math.h>
#include <Render/Buffer/Framebuffer.h>
#include <Tools/Array.h>
class QCameraComponent;
class QFAWindow;
class QFAFrameBufferMain;
class QSceneComponent;
class QWorld;
class QMeshBaseComponent;
class QFARenderText;

//#include <Render/Text/Text.h>

class QFAViewport
{
	

	friend QFAWindow;
	friend QFAFrameBufferMain;
	// first create ViewPort
	static QFAViewport* DefaultViewPort;
	glm::mat4 MatrixPerspective;
	QCameraComponent* CurentCamera = nullptr;

	// rename
	QFAFrameBuffer secondFrameBuffer;
	// offset from left bottom corner
	int X, Y;
	int Width = 1;
	int Height = 1;
	int WindowWidth = 1, WindowHeight = 1;
	float XP = 0, YP = 0;
	float WidthP = 1;
	float HeightP = 1;

	uint64_t StartFrameTime;
	
	QWorld* CurentFrameWorld;


	QFAArray<QFARenderText*> Texts;
	

	void Settup(int windowWidth, int windowHeight);
	void ProcessFrame(uint64_t startFrameTime);
	bool IsActive = true;
	void ProcessComponent(QSceneComponent* component);
	void ProcessComponentShadow(QSceneComponent* component);
	void DrawMesh(QMeshBaseComponent* mesh);
	void DrawMeshShadow(QMeshBaseComponent* mesh);
public:
	QFAViewport();
	~QFAViewport();

	inline void ActivateCamera();

	static inline QFAViewport* GetDefaultViewport()
	{
		return DefaultViewPort;
	}
	void ChangeCamera(QCameraComponent* camera);

	/*
		p = percent
		range 0 - 1

		start left bottom corner(like in opengl)

		xP offset from left side in persent window width
		widthP width viewport in persent window width
	*/
	void SetParameters(float xP, float  yP, float widthP, float heightP);

	void AddText(QFARenderText* text);
	void RemoveText(QFARenderText* text);
private:

};
