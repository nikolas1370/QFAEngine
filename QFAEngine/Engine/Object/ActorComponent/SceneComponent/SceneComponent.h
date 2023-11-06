#pragma once
#include <Object/ActorComponent/ActorComponent.h>
#include <Math/Vector.h>
#include <Math/Math.h>


class QMeshBaseComponent;

/*
	relative position  // relative to parent
	local position     // relative to Actore
	world position
*/

class QFARender;
class QSceneComponent : public QActorComponent
{

	// remove
	friend QFARender;

	friend QActor;
	friend QSceneComponent;
	friend QMeshBaseComponent;

	//void UpdateModelMatrix();
	QActor* ParentActor;
	QSceneComponent* ParentActorComponent;
	bool IRootComponent = false;



	unsigned int ListLength = 0;
	
	

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

	static inline  FVector ToOpenglCoordinate(FVector positon)
	{
		return FVector(positon.Y, positon.Z, positon.X * -1);
	}

	static inline FVector FromOpenglCoordinate(FVector positon)
	{
		return FVector(positon.Z * -1, positon.X, positon.Y);
	}

	// in opengl format
	FVector WorldPosition = FVector(0);
	// in opengl format
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
	
	/*
	* call inside QSceneComponent and from some child
	*/
	void ChangeWorldPosition(const FVector position);
	void ChangeLocalPosition(const FVector position);
	void ChangeRelativePosition(const FVector position);
	void ChangeRotation(const FVector rotation);	
	void ChangeScale(const FVector scale);
	/*----*/

	// if component can be render
	bool ForRender = false;


	virtual void UpdateModelMatrix(bool onlyPosition) {};
public:
	QSceneComponent(); 


	// in private
	unsigned int CountComponent = 0;
	// in private
	QSceneComponent** ListComponents;

	/*
	do delete all child component
	*/
	~QSceneComponent();

	/*
	*	if component is RootComponent position set in QActor
	*   if component has not parent the position not set
	*	if component has parent but not conect to QActor tree position set incorrectly		 
	*/
	virtual inline void SetWorldPosition(const FVector position)
	{
		ChangeWorldPosition(position);
	}
	
	inline FVector GetWorldPosition() const
	{
		return FromOpenglCoordinate(WorldPosition);// FVector(ModelMatrix[3][2] * -1, ModelMatrix[3][0], ModelMatrix[3][1]);
	}

	/*
	*	if component is RootComponent the position doesn't set
	*   if component has not parent the position not set
	*	if component has parent but not conect to QActor tree position set incorrectly		 
	*/
	virtual inline void SetLocalPosition(const FVector position)
	{
		ChangeLocalPosition(position);
	}
	// ned be inline
	FVector GetLocalPosition() ;

	/*
	*	if component is RootComponent the position doesn't set
	*   if component has not parent the position not set
	*/
	virtual inline void SetRelativePosition(const FVector position)
	{
		ChangeRelativePosition(position);
	}

	inline FVector GetRelativePosition() const
	{
		return RelativePosition;
	}

	
	virtual inline void SetRotation(const FVector rotation)
	{
		ChangeRotation(rotation);
	}
	virtual inline FVector GetRotation() const
	{
		return Rotation;
	}


	virtual inline void SetScale(const FVector scale)
	{
		ChangeScale(scale);
	}
	FVector GetScale() const;

	
	virtual FVector GetForwardVector() const; // GetActorForwardVector
	virtual FVector GetRightVector() const;
	virtual FVector GetUpVector() const;
	
	void AttachComponent(QSceneComponent* component);
	inline QSceneComponent* GetParentComponent()
	{
		return ParentActorComponent;
	}

	/*
		return true if component was found and delete from ListComponents
	*/
	bool ForgetComponent(QSceneComponent* component);

	inline QActor* GetActor() 
	{
		return QSceneComponent::GetActor(this);
	}

	/*
		get actor who owns component

		можливо зроблю шоб при добавляні компонента актор давався
	*/
	static QActor* GetActor( QSceneComponent* component) ;
private:

	
};

