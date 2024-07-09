#pragma once
#include <Math/Math.h>
#include <Object/World/World.h>
#include <Tools/Array.h>
#include <Render/Window/QFAViewportRoot.h>
class QCameraComponent;
class QFAWindow;

class QSceneComponent;
class QWorld;
class QMeshBaseComponent;

class QFAUIUnit;
class QFAUIParentMultipleUnit;
class QFAEditorMainWindow;
class QFAEXPORT QFAViewport
{
	friend QFAWindow;
	friend QFAUIParentMultipleUnit;
	friend QFAUIUnit;
	friend QFAEditorMainWindow;

public:
	static const float MinMaxZIndexUI;

protected:
	/*
		in this buffer store  QMeshBaseComponent::UBOVertex
		use for render mesh
	*/
	static std::vector<QFAVKBuffer*> MeshVertexBuffers;

private:
	QFAWindow* Window;
	glm::mat4 MatrixPerspective;
	QCameraComponent* CurentCamera = nullptr;
	int X, Y;
	int Width = 1;
	int Height = 1;
	int WindowWidth = 1, WindowHeight = 1;
	float XP = 0, YP = 0;
	float WidthP = 1;
	float HeightP = 1;
	uint64_t StartFrameTime;
	QWorld* CurentFrameWorld;
	glm::mat4 UIProjection;
	bool IsActive = true;
	QFAViewportRoot Root;

private:
	void Settup(int windowWidth, int windowHeight);

protected:
	void WindowAddMe(QFAWindow* window);
	void WindowRemoveMe();

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

	inline void AddUnit(QFAUIUnit* unit)
	{
		Root.AddUnit(unit);
	}

	inline void RemoveUnit(QFAUIUnit* unit)
	{
		Root.removeUnit(unit);
	}

	inline QFAWindow* GetWindow()
	{
		return Window;
	}
	
	QWorld* GetWorld();

	inline QFAViewportRoot* GetRoot()
	{
		return &Root;
	}

	inline FVector2D GetSize()
	{
		return FVector2D(Width, Height);
	}

	inline FVector2D GetPosition()
	{
		return FVector2D(X, Y);
	}
};
