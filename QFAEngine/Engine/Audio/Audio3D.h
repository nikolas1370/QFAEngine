#pragma once
#include <Audio/Audio.h>
#include <x3daudio.h>
class QFAEXPORT QFAAudio3D
{
	static X3DAUDIO_HANDLE X3DInstance;
	static bool IsInit;
	QFAAudio Audio;	
	X3DAUDIO_EMITTER Emitter{};
	X3DAUDIO_LISTENER Listener = {};
	FLOAT32 Matrix[16] = {};
	X3DAUDIO_CONE SoundCone; 
	float DopplerScaler = 100;
	float DistanceScaler = 100;
	float InnerRadius = 1;
	float InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
	static void Init();

public:
	QFAAudio3D();
	QFAAudio3D(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400);
	~QFAAudio3D();

	void SetPositionListener(FVector position, bool needUpdate = false);
	void SetPositionEmitter(FVector position, bool needUpdate = false);
	void SetVelocityListener(FVector velocity, bool needUpdate = false);
	void SetVelocityEmitter(FVector velocity, bool needUpdate = false);
	// set forvard and up vector listener
	void SetFUVectorListener(FVector forvard, FVector up, bool needUpdate = false);
	void SetDopplerScaler(float scale, bool needUpdate = false);
	void SetDistanceScaler(float scale, bool needUpdate = false);
	// inner radius, must be within [0.0f, FLT_MAX]
	void SetInnerRadius(float radius, bool needUpdate = false);
	// inner radius angle, must be within [0.0f, X3DAUDIO_PI/4.0)
	void SetInnerRadiusAngle(float Angle, bool needUpdate = false);
	void UpdateParameters();

	void SetAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400);

	inline void Play()
	{
		Audio.Play();
	}

	inline void Stop()
	{
		Audio.Stop();
	}

	inline void SetRepeat(const bool repeat)
	{
		Audio.SetRepeat(repeat);
	}

	inline bool GetRepeat()
	{
		return Audio.GetRepeat();
	}

	inline void SetVolume(const float volume)
	{
		Audio.SetVolume(volume);
	}

	inline size_t GetMaxTime()
	{
		return Audio.Loader.MaxTime;
	}

	inline size_t GetTime()
	{
		return Audio.GetTime();
	}

	inline void SetTime(size_t millisecond)
	{
		Audio.SetTime(millisecond);
	}

	inline bool IsPlay()
	{
		return Audio.AudioPlay;
	}

	inline void SetStartTime(size_t millisecond)
	{
		Audio.SetStartTime(millisecond);
	}

	inline void SetEndTime(size_t millisecond)
	{
		Audio.SetEndTime(millisecond);
	}
};