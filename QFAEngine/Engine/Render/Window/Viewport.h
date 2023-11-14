#pragma once
#include <Math/Math.h>
class QFAFrameBuffer;
class QCameraComponent;
class QFAWindow;
class QFAFrameBufferMain;
class QSceneComponent;
class QWorld;
class QMeshBaseComponent;
class QFAViewport
{
	friend QFAWindow;
	friend QFAFrameBufferMain;
	// first create ViewPort
	static QFAViewport* DefaultViewPort;
	glm::mat4 MatrixPerspective;
	QCameraComponent* CurentCamera = nullptr;

	// rename
	QFAFrameBuffer* secondFrameBuffer;
	// offset from left bottom corner
	int X, Y;
	int Width = 1;
	int Height = 1;

	QWorld* CurentFrameWorld;

	void Settup(int x, int y, int width, int height);
	void ProcessFrame();
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
private:

};
