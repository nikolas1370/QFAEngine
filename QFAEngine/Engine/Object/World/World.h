#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Tools/Array.h>
class QActor;
class QFAOverlord;
class QFAEngineViewport;
class QSceneComponent;
class QFAEngineWindow;
class QFALevel;
class QFAEXPORT QWorld : public QObject
{
	QFAEngineClassIn();
#if QFA_EDITOR_ONLY
	friend QActor;// for QActor::ReplaceMe
#endif 
	
	friend QFAOverlord;
	friend QActor;
	friend QFAEngineViewport;
	friend QFAEngineWindow;
	friend QFALevel;

protected:
	QDirectionLight DirectionLight;
	static QFAArray<QWorld*> Worlds;
	bool IsActive = true;

	/*
		if Enable == false tick provces in EditorActor
	
	*/
	QActor* EditorActor = nullptr;
	bool Enable = true;
	QFAArray<QActor*> Actors;

	static void ProcessTicks();
	static void ProcessSceneComponentTick(QSceneComponent* component);
#if QFA_EDITOR_ONLY
	void SetEditorActor(QActor* actor);
	
#endif 
	
public:

	QWorld();
	~QWorld();

	void AddActor(QActor* actor);
	void RemoveActor(QActor* actor);

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

	inline size_t GetActorSize()
	{
		return Actors.Length();
	}

	void DestroyWorld(bool deleteAllActor = true);
};
