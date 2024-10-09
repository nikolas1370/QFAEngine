#pragma once
#include <Tools/Stuff.h>
#include <EngineStuff/Audio/AudioLoader.h>

class QFAAudio3D;
class QFAEXPORT QFAAudio
{
	friend QFAAudio3D;
	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		QFAAudio* QAudio = nullptr;
		HANDLE hBufferEndEvent;
		VoiceCallback();
		
		~VoiceCallback();
		//Called when the voice has just finished playing a contiguous audio stream.
		void OnStreamEnd() override {  }
		void OnVoiceProcessingPassEnd() override { }
		void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override  {    }
		void OnBufferEnd(void* pBufferContext) override;
		void OnBufferStart(void* pBufferContext) override {    }
		void OnLoopEnd(void* pBufferContext) override {    }
		void OnVoiceError(void* pBufferContext, HRESULT Error) override { }
	};
	
	friend VoiceCallback;


	// The IXAudio2 interface is the core of the XAudio2 engine.
	static IXAudio2* XAudio2; //can create multiple XAudio2

	/*
		Voices are the objects XAudio2 use to process, 
		to manipulate, and to play audio data. 
		There are three types of voices in XAudio2.

		Source Voices:
			Source voices represent a stream of audio data.
			Source voices send their data to other types of voices.

		Submix Voices:
			Submix voices perform some manipulation of audio data they receive.
			One example of audio data manipulation might be sample rate conversion.
			After a submix voice processes data,
			it passes that data to another submix voice or to a master voice.

		Mastering Voices:
			Mastering voices receive data from source voices and submix voices,
			and sends that data to the audio hardware.

		See XAudio2 Voices for an overview of XAudio2 voices.
		https://learn.microsoft.com/en-us/windows/win32/xaudio2/voices
	*/

	QFAAudioLoader Loader;

	// A mastering voice is used to represent the audio output device.
	static IXAudio2MasteringVoice* XAudio2MasteringVoice;
	/// onen audio one IXAudio2SourceVoice
	IXAudio2SourceVoice* XAudio2SourceVoice = nullptr;

	VoiceCallback VoiceCallback;




	bool AudioPlay = false;
	
	bool Repeat = false;
	static void Init();
	static DWORD GetChannelMask();

	void BufferEnd();
	void RecreateVoice();
	XAUDIO2_BUFFER& ReadBuffer(bool next = true, size_t millisecond = 0);
	inline size_t GetCurentFrame()
	{
		return (size_t)((double)GetTime() / Loader.FrameTime);
	}

public:
	// if isAudioStream == false all file be store in memory
	// if isAudioStream == true buffer be have size bufferSize
	QFAAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400);// 100kb
	QFAAudio();
	~QFAAudio();
	void SetAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize = 102400);
	void Play();
	void Stop();
	void SetRepeat(const bool repeat)
	{
		Repeat = repeat;
	}

	bool GetRepeat()
	{
		return Repeat;
	}

	void SetVolume(const float volume);

	// get length of audio in ms
	inline size_t GetMaxTime()
	{
		return Loader.MaxTime;
	}
	/*
		get curent play time in millisecond
	*/
	size_t GetTime();
	/*
		set curent play time in millisecond
		if millisecond < SetStartTime audio be play from SetStartTime
		if millisecond >= SetEndTime and Repeat == true play from SetStartTime if not ignore
	*/
	void SetTime(size_t millisecond);

	inline bool IsPlay()
	{
		return AudioPlay;
	}

	/*
		Audio start play from millisecond
		if current time < millisecond time will set in millisecond
		if millisecond >= SetEndTime value be ignored
	*/
	inline void SetStartTime(size_t millisecond)
	{
		Loader.SetStartTime(millisecond);
	}
	

	/*	
		Audio end play in millisecond
		if millisecond <= SetStartTime value be ignored
	*/
	inline void SetEndTime(size_t millisecond)
	{
		Loader.SetEndTime(millisecond);
	}
	
};
