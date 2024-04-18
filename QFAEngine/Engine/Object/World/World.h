#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Tools/Array.h>
class QActor;
class QFAOverlord;
class QFAViewport;
class QSceneComponent;
class QFAEditorMainWindow;


	QFAClass
class QWorld : public QObject
{

	static int lox;

	friend QFAOverlord;
	friend QFAEditorMainWindow;
	
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

	/*
		if Enable == false tick provces in EditorActor
	
	*/
	QActor* EditorActor = nullptr;
	bool Enable = true;
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

	
	inline bool GetEnable()
	{
		return Enable;
	}

	inline void SetEnable(bool enable)
	{
		Enable = enable;
	}
private:
	void SetEditorActor(QActor* actor);
};
