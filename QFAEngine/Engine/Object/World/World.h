#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Tools/Array.h>
class QActor;
class QFAOverlord;
class QFAViewport;
class QSceneComponent;


class QWorld : public QObject
{
	friend QFAOverlord;

	
public:


private:
	
	


	friend QActor;
	friend QFAViewport;
	QDirectionLight DirectionLight;

	static QFAArray<QWorld*> Worlds;

	// call only from QActor Destructor
	void ForgetActor(QActor* actor);

	bool IsActive = true;
	static void ProcessTicks();
	static void ProcessSceneComponentTick(QSceneComponent* component);

	QFAViewport* Viewport = nullptr;
public:
	

	QFAArray<QActor*> Actors;

	/*--*/


	QWorld();
	~QWorld();

	void AddActor(QActor* actor);
	
	inline QDirectionLight* GetDirectionDight()
	{
		return &DirectionLight;
	}
private:

};
