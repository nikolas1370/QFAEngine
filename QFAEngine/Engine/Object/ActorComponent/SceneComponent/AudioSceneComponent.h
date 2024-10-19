#pragma once
#include <Object/ActorComponent/SceneComponent/SceneComponent.h>
#include <Audio/Audio3D.h>

class QWorld;
class QCameraComponent;
// need activate audio in QCameraComponentin
// and QAudioSceneComponent need be in same world with QCameraComponentin
class QFAEXPORT QAudioSceneComponent : public QSceneComponent
{	
	QFAEngineClassIn(QAudioSceneComponent);
	friend QCameraComponent;
	friend QWorld;
	QWorld* AudioWorld = nullptr;
	QFAAudio3D Audio;
	bool ShouldPlay = false;
protected:
	void ParentChanged() override;
	void UpdateModelMatrix() final;
	void UpdateEmitterListenerParameters();
	void StopEngine();

public:
	QAudioSceneComponent();
	QAudioSceneComponent(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400);
	~QAudioSceneComponent();

	void Play();
	void Stop();
	inline void SetRepeat(const bool repeat)
	{
		Audio.SetRepeat(repeat);
	}

	inline void SetAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400)
	{
		Audio.SetAudio(fileName, isAudioStream, bufferSize);
	}

	inline void SetEndTime(size_t millisecond)
	{
		Audio.SetEndTime(millisecond);
	}

};