#pragma once
#include <Object/Object.h>

#include "Math/Vector.h"
class Render;
class QWorld;
class QSceneComponent;
class QActor : public QObject
{
	/*
	for write on position rotation scale
	*/
	friend QSceneComponent;

	
	// SceneComponent

	FVector Position = FVector(0);
	FVector Rotation = FVector(0);
	FVector Scale = FVector(1);
	
	//friend Render;
	QWorld* ActorWorld;
public:

	// in private
	QSceneComponent* RootComponent;


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


	virtual FVector GetActorForwardVector() const; 
	virtual FVector GetActorRightVector() const;
	virtual FVector GetActorUpVector() const;

	inline QSceneComponent* GetRootComponent()
	{
		return RootComponent;
	}
	/*
		if component == null. RootComponent = null
		���� ��������� ��������� �������� ������� ������ �� ���������� �� �����/��������� ������ ��� �����
		return old RootComponent
	*/
	QSceneComponent* SetRootComponent(QSceneComponent* component);


	//will return null if the actor is not actually spawned in a level
	inline QWorld* GetWorld()
	{
		return ActorWorld;
	}

};

