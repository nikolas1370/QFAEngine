#include "pch.h"
#include "AudioSceneComponent.h"
#include <Object/World/World.h>
#include <Object/ActorComponent/SceneComponent/Camera/Camera.h>

QFAEngineClassOut(QAudioSceneComponent);
QAudioSceneComponent::QAudioSceneComponent()
{
}

QAudioSceneComponent::QAudioSceneComponent(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{
	Audio.SetAudio(fileName, isAudioStream, bufferSize);
}

QAudioSceneComponent::~QAudioSceneComponent()
{
	if(QWorld* audioWorld = GetWorld())
		audioWorld->RemoveAudio(this);
}

void QAudioSceneComponent::Play() 
{	
	if (QCameraComponent* mainCam = QCameraComponent::GetMainCamera())
		if (QWorld* camWorld = mainCam->GetWorld())
			if (AudioWorld == camWorld && !camWorld->SilenceWorld)
				Audio.Play();

	ShouldPlay = true;
}

void QAudioSceneComponent::Stop() 
{	
	Audio.Stop();
	ShouldPlay = false;
}

void QAudioSceneComponent::StopEngine()
{
	if(Audio.IsPlay())
		ShouldPlay = true;
}

void QAudioSceneComponent::ParentChanged()
{
	QSuper::ParentChanged();
	if (AudioWorld)
		AudioWorld->RemoveAudio(this);	

	if (QSceneComponent* parent = GetParentComponent())
	{
		if (QWorld* w = parent->GetWorld())
		{
			w->AddAudio(this);
			AudioWorld = w;
			if (ShouldPlay && !Audio.IsPlay())
				Play();

			return;
		}	
	}
	
	AudioWorld = nullptr;
	Stop();
}

void QAudioSceneComponent::UpdateModelMatrix()
{
	UpdateEmitterListenerParameters();
}

void QAudioSceneComponent::UpdateEmitterListenerParameters()
{
	if(QCameraComponent::GetMainCamera())
	{
		Audio.SetPositionEmitter(GetWorldPosition(), false);
		Audio.SetPositionListener(QCameraComponent::GetMainCamera()->GetWorldPosition(), false);
		Audio.SetFUVectorListener(QCameraComponent::GetMainCamera()->GetForwardVector(), QCameraComponent::GetMainCamera()->GetUpVector(), true);		
	}
}
