#pragma once
#include <Math/Math.h>

#include <Tools/Array.h>
class QCameraComponent;
class QFAWindow;

class QSceneComponent;
class QWorld;
class QMeshBaseComponent;

class QFAUIUnit;
class QFAUIParentComponent;

class QFAViewport
{


	friend QFAWindow;
	friend QFAUIParentComponent;
	friend QFAUIUnit;

	QFAWindow* Window;

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


	QFAArray<QFAUIUnit*> UIUnits;
	glm::mat4 UIProjection;



	void Settup(int windowWidth, int windowHeight);	
	bool IsActive = true;

public:
	QFAViewport();
	~QFAViewport();

	void ActivateCamera();
	void DeactivateCamera();

	// if camera change some parameter
	// 0 not camera, 1 camera activate, 2 camera deactivate
	// rewrite to QCameraComponent::Parameter
	void CameraChangeParameter(int param);

	
	void ChangeCamera(QCameraComponent* camera);

	/*
		p = percent
		range 0 - 1

		start left bottom corner(like in opengl)

		xP offset from left side in persent window width
		widthP width viewport in persent window width
	*/
	void SetParameters(float xP, float  yP, float widthP, float heightP);

	void AddUnit(QFAUIUnit* unit);
	void RemoveUnit(QFAUIUnit* unit);
protected:
	// inside function
	void RemoveUnitWithoutNotify(QFAUIUnit* unit); // , bool notifyUnit

	void WindowAddMe(QFAWindow* window);
	void WindowRemoveMe();
};
