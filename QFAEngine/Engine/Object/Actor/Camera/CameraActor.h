#pragma once
#include <Object/Actor/Actor.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>
#include <Math/Vector.h>
#include <Math/Vector2D.h>

class QFAEXPORT ACameraActor : public QActor
{
	QFAEngineClassIn(ACameraActor);
protected:
	QCameraComponent Camera = QCameraComponent(45.0f, 100000.0f);
	
public:
	ACameraActor();
	~ACameraActor();

	inline void SetFov(float fov)
	{
		Camera.SetFov(fov);
	}
	inline void SetViewDistance(float viewDistance)
	{
		Camera.SetViewDistance(viewDistance);
	}
	inline float GetFov()
	{
		return Camera.GetFov();
	}
	inline float GetViewDistance()
	{
		return Camera.GetViewDistance();
	}

	void ActivateCamera(QFAEngineViewport* viewport = nullptr);
};
