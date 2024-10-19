#pragma once
#include <Object/Object.h>
#include <Object/World/DirectionLight/DirectionLight.h>
#include <Tools/Array.h>
#include <Object/ActorComponent/SceneComponent/AudioSceneComponent.h>
#include <Math/Vector.h>
class QActor;
class QFAOverlord;
class QFAEngineViewport;
class QSceneComponent;
class QFAEngineWindow;
class QFALevel;
class QCameraComponent;
class QFAEXPORT QWorld : public QObject
{
	QFAEngineClassIn(QWorld);
#if QFA_EDITOR_ONLY
	friend QActor;// for QActor::ReplaceMe
#endif 
	
	friend QFAOverlord;
	friend QActor;
	friend QFAEngineViewport;
	friend QFAEngineWindow;
	friend QFALevel;
	friend QAudioSceneComponent;
	friend QCameraComponent;

	struct SAudio
	{
		QAudioSceneComponent* AudioComponent;
		FVector LastAudioLocation = {}; 
	};

	std::vector<SAudio> AudioComponents;

protected:
	QDirectionLight DirectionLight;
	
	static QFAArray<QWorld*> Worlds;
	QActor* EditorActor = nullptr;
	QFAArray<QActor*> Actors;

	/*
		if true QActor::Tick() be called in all Actors
		otherwise call EditorActor->Tick()
	*/
	bool Enable = true;
	bool IsActive = true;
	// if true QAudioSceneComponent can't play need for editor
	bool SilenceWorld = false;


	static void ProcessTicks();
	static void ProcessSceneComponentTick(QSceneComponent* component);
#if QFA_EDITOR_ONLY
	void SetEditorActor(QActor* actor);
	
#endif 
	
	// QAudioSceneComponent call it if parent component change and parent live in this world
	void AddAudio(QAudioSceneComponent* audio);
	// QAudioSceneComponent call it if parent component change or parent not exist
	void RemoveAudio(QAudioSceneComponent* audio);
	
	/*
		call in QCameraComponent::ActivateAudio
		New Audio set in QCameraComponent::MainCamera
	*/
	void NewAudioActive(QCameraComponent* oldCamera);
	void StopAndClearAudio();
	void SearchAndAddAudioInList(QSceneComponent* scene);
	void SearchAndDeleteAudioInList(QSceneComponent* scene);

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

	virtual void DestroyWorld(bool deleteAllActor = true) final;
};
