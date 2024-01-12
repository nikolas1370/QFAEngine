#pragma once
#include <Math/Math.h>

#include <Tools/Array.h>
class QCameraComponent;
class QFAWindow;

class QSceneComponent;
class QWorld;
class QMeshBaseComponent;
class QFAText;

class QFAViewport
{


	friend QFAWindow;

	// first create ViewPort
	static QFAViewport* DefaultViewPort;
	glm::mat4 MatrixPerspective;
	QCameraComponent* CurentCamera = nullptr;

	// rename

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


	QFAArray<QFAText*> Texts;
	glm::mat4 UIProjection;



	void Settup(int windowWidth, int windowHeight);	
	bool IsActive = true;

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

	void AddText(QFAText* text);
	void RemoveText(QFAText* text);
private:

};
