#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Tools/Array.h>
class QActor;
class QFAOverlord;
class QFAViewport;
class QSceneComponent;
class QFAEditorMainWindow;



class QFAEXPORT QWorld : public QObject
{

#if QFA_EDITOR_ONLY
	friend QActor;// for QActor::ReplaceMe
#endif 
	

	friend QFAOverlord;
	friend QFAEditorMainWindow;
	
public:


private:
	
	


	friend QActor;
	friend QFAViewport;
	QDirectionLight DirectionLight;

	static QFAArray<QWorld*> Worlds;
	
	

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


private:
	void SetEditorActor(QActor* actor);
};
