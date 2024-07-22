#pragma once
#include <Object/Object.h>

#include "Math/Vector.h"
class Render;
class QWorld;
class QSceneComponent;
class QFAOverlord;
class QMeshBaseComponent;
class QFAGameCode;
class QFAEngineWindow;
class QFALevel;
class QFAEXPORT QActor : public QObject
{
	QFAEngineClassIn();
	/*
	for write on position rotation scale
	*/
	friend QSceneComponent;
	friend QWorld;
	friend QFAOverlord;
	friend QMeshBaseComponent;
	friend QFAGameCode;
	friend QFAEngineWindow;
	friend QFALevel;


	QSceneComponent* RootComponent = nullptr;

	// pointer to this actor in QWorld::Actors
	size_t WorldIndex;
#if QFA_EDITOR_ONLY
	bool NameRedacted = false; // QFALevel::SaveLevel
#endif 
protected:
	FVector Position = FVector(0);
	FVector Rotation = FVector(0);
	FVector Scale = FVector(1);

	bool CanTick = false;
	// actor live in this world
	QWorld* ActorWorld = nullptr;

private:

#if QFA_EDITOR_ONLY
	/*
		used only for HotReload
		replace this actor in parent world to actor
	*/
	void ReplaceMe(QObject* newActor) override;
#endif 
protected:
	virtual void Tick(float deltaTime) {};

public:
	QActor();
	~QActor();

	void SetActorPosition(const FVector& position);
	
	inline FVector GetActorPosition() const
	{
		return Position;
	}

	void SetActorRotation(const FVector& rotation);
	
	inline FVector GetActorRotation() const
	{
		return Rotation;
	}

	void SetActorScale(const FVector& scale);

	inline FVector GetActorScale() const
	{
		return Scale;
	}

	FVector GetActorForwardVector() const; 
	FVector GetActorRightVector() const;
	FVector GetActorUpVector() const;

	inline QSceneComponent* GetRootComponent()
	{
		return RootComponent;
	}
	/*
		if component == null. RootComponent = null
		return old RootComponent

		if component Inseparable return nullptr
	*/
	bool SetRootComponent(QSceneComponent* component, bool inseparable = false);

	//will return null if the actor is not actually spawned in world
	inline QWorld* GetWorld()
	{
		return ActorWorld;
	}

	inline void SetTick(bool enable)
	{
		CanTick = enable;
	}
};

