#pragma once
#include <Math/Math.h>
#include <Object/World/World.h>
#include <Tools/Array.h>
#include <EngineStuff/Window/QFAViewportRoot.h>
#include <Window/Window.h>
class QCameraComponent;
class QFAEngineWindow;

class QSceneComponent;
class QWorld;
class QMeshBaseComponent;

class QFAUIUnit;
class QFAUIParentMultipleUnit;
class QFAEditorMainWindow;
class QFAViewportHolder;
class QFAWindow;
class QStaticMesh;
class QFAEXPORT QFAEngineViewport
{
	friend QFAEngineWindow;
	friend QFAUIParentMultipleUnit;
	friend QFAUIUnit;
	friend QFAEditorMainWindow;
	friend QFAViewportHolder;
	friend QFAWindow;
	friend QStaticMesh;

public:
	static const float MinMaxZIndexUI;

protected:
	/*
		in this buffer store  QMeshBaseComponent::UBOVertex
		use for render mesh
	*/
	static std::vector<QFAVKBuffer*> MeshVertexBuffers;
#if QFA_EDITOR_ONLY
	// set it if start game in editor
	static bool InGame;
	static void SetInGame(bool ingame);
	static bool GetInGame();
#endif
private:
	glm::mat4 MatrixPerspective;
	QCameraComponent* CurentCamera = nullptr;

	int WindowWidth = 1, WindowHeight = 1;

	const char* TopChars = nullptr;
	const char* LeftChars = nullptr;
	const char* HeightChars = nullptr;
	const char* WidthtChars = nullptr;

	uint64_t StartFrameTime;
	QWorld* CurentFrameWorld;
	glm::mat4 UIProjection;
	bool IsActive = true;
	QFAViewportRoot Root;

protected:
	int Width = 1, Height = 1; // not use directly call GetPosition
	int X, Y;// not use directly call GetPosition

	QFAWindow* Window;
#if QFA_EDITOR_ONLY
	bool RegularViewport = true;
#endif

private:
	void UpdateX();
	void UpdateY();
	void UpdateHeight();
	void UpdateWidth();
	
protected:
	void WindowAddMe(QFAWindow* window);
	void WindowRemoveMe();
	void SettupInside(int windowWidth, int windowHeight);
	// need in QFAEngineWindow, 
	virtual void Settup(int windowWidth, int windowHeight)
	{
		SettupInside(windowWidth, windowHeight);
	}
	QFAEngineViewport();
	~QFAEngineViewport();

public:
	void ActivateCamera();
	void DeactivateCamera();

	// if camera change some parameter
	// 0 not camera, 1 camera activate, 2 camera deactivate
	// rewrite to QCameraComponent::Parameter
	void CameraChangeParameter(int param);

	void ChangeCamera(QCameraComponent* camera);

	// "17.1% - 5.1 + 2" corect input	
	void SetHeight(const char* height);
	void SetWidth(const char* width);	
	void SetTop(const char* top);
	void SetLeft(const char* left);


	FVector2D GetPosition();

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
};
