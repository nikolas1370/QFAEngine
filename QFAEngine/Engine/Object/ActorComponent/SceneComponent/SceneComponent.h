#pragma once
#include <Object/ActorComponent/ActorComponent.h>
#include <Math/Vector.h>
#include <Math/Math.h>
#include <Tools/Array.h>

class QMeshBaseComponent;
class QFAOverlord;

/*
	relative position  // relative to parent
	local position     // relative to Actore
	world position
*/
class QFAEXPORT QSceneComponent : public QActorComponent
{
	QFAEngineClassIn();
	friend QActor;
	friend QSceneComponent;
	friend QMeshBaseComponent;
	friend QFAOverlord;

	QActor* ParentActor = nullptr;
	QSceneComponent* ParentActorComponent = nullptr;
	bool IRootComponent = false;

	/*
	can't be detached
	will not detach
*/
	bool Inseparable = false;

protected:
	// in vulkan format
	FVector WorldPosition = FVector(0);
	// normal format. relative to actor
	FVector LocalPosition = FVector(0);

	// in vulkan format
	glm::mat3 RotationMatrix = Math::DefauldMatrix3;
	/*
	 contain parent AccumulateScale * parent scale
		RootComponent have in AccumulateScale QActor scale
	*/
	FVector AccumulateScale = FVector(1);

	// relative parent
	FVector RelativePosition = FVector(0);
	FVector Rotation = FVector(0);
	FVector Scale = FVector(1);
	// before be in MeshBase

	bool NeedUpdateMatrix = false;
public:
	/*
		get actor who owns component
	*/
	static QActor* GetActor(QSceneComponent* component);

private:

	/*
		if component exist higher up in tree (parend, grandparent, grandgrandparent...)
	*/
	bool ItsMyFather(QSceneComponent* component);

	// call from parent component when Rotation/RotationMatrix change or one of parent be attached
	void ChangedParentRotation();

	/**/
	void UpdateWorldPositionScale(bool onlyPosition);

	inline FVector GetRelativeScalePosition()
	{
		return RelativePosition * AccumulateScale;
	}
protected:
	/*
	* call inside QSceneComponent and from some child
	*/
	void ChangeWorldPosition(const FVector position);
	void ChangeLocalPosition(const FVector position);
	void ChangeRelativePosition(const FVector position);
	void ChangeRotation(const FVector rotation);	
	void ChangeScale(const FVector scale);

	virtual void UpdateModelMatrix() {};
public:

	// remove to private
	QFAArray<QSceneComponent*> ListComponents;

	QSceneComponent(); 

	inline bool IsRootComponent()
	{
		return IRootComponent;
	}

	inline glm::mat3 GetRotationMatrix()
	{
		return RotationMatrix;
	}

	/*
	do delete all child component
	*/
	~QSceneComponent();

	/*
	*	if component is RootComponent position set in QActor
	*   if component has not parent the position not set
	*	if component has parent but not conect to QActorTree position set incorrectly		 
	*/
	 inline void SetWorldPosition(const FVector position)
	{
		ChangeWorldPosition(position);
	}
	
	inline FVector GetWorldPosition() const
	{		
		return WorldPosition.ConvertFromVulkanCoordinate() ;
	}

	/*
	*	if component is RootComponent the position doesn't set
	*   if component has not parent the position not set
	*	if component has parent but not conect to QActor tree position set incorrectly		 
	*/
	 inline void SetLocalPosition(const FVector position)
	{
		ChangeLocalPosition(position);
	}
	// ned be inline
	FVector GetLocalPosition() ;

	/*
	*	if component is RootComponent the position doesn't set
	*   if component has not parent the position not set
	*/
	 inline void SetRelativePosition(const FVector position)
	{
		ChangeRelativePosition(position);
	}

	inline FVector GetRelativePosition() const
	{
		return RelativePosition;
	}

	
	 inline void SetRotation(const FVector rotation)
	{
		ChangeRotation(rotation);
	}
	 inline virtual FVector GetRotation() const
	{
		return Rotation;
	}


	 inline void SetScale(const FVector scale)
	{
		ChangeScale(scale);
	}
	FVector GetScale() const;

	
	virtual FVector GetForwardVector() const; // GetActorForwardVector
	virtual FVector GetRightVector() const;
	virtual FVector GetUpVector() const;
	
	void AttachComponent(QSceneComponent* component, bool inseparable = false);
	inline QSceneComponent* GetParentComponent() const
	{
		return ParentActorComponent;
	}

	/*
		return true if component was found and delete from ListComponents
			false if not found or component Inseparable
	*/
	bool ForgetComponent(QSceneComponent* component);

	inline QActor* GetActor() 
	{
		return QSceneComponent::GetActor(this);
	}

	QWorld* GetWorld();
};

