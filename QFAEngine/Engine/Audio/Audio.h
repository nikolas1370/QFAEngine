#pragma once
#include <Tools/Stuff.h>
#include <xaudio2.h>
#include <Tools/File/FileSystem.h>
#include "vector"

class QFAEXPORT QFAAudio
{
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

	std::u32string FileName;
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
	// A mastering voice is used to represent the audio output device.
	static IXAudio2MasteringVoice* XAudio2MasteringVoice;
	IXAudio2SourceVoice* XAudio2SourceVoice = nullptr;

	WAVEFORMATEXTENSIBLE Wfx;

	int MaxBuffersize;

	VoiceCallback VoiceCallback;

	QFAFile FIle; // for not stream
	bool UseFirstBuffer = true;
	XAUDIO2_BUFFER Buffers[2];	
	QFAFileReadStream FileStream; // for stream
	bool IsAudioStream;
	size_t DataSize;
	size_t DataOffset; // offset from start file; use if IsAudioStream == true	
	bool Repeat = false;	
	size_t MaxTime = 0;// in millisecond
	double FrameTime = 0.0; // in millisecond
	bool AudioPlay = false;
	size_t PlayStartFrom = 0; // Play Start From this Sample(frame) need for GetTime if use SetTime
	unsigned char FrameSize;// in 2 channel with sample 2 byte be Frame be 4 bytes


	bool GetWavInfo();
	void LoadWholeFile();
	void LoadStreamFile(XAUDIO2_BUFFER& buffer);
	void BufferEnd();
public:
	// if isAudioStream == false all file be store in memory
	// if isAudioStream == true buffer be have size bufferSize
	QFAAudio(const std::u32string& fileName, bool isAudioStream, size_t bufferSize = 102400);// 100kb
	~QFAAudio();

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
		return MaxTime;
	}
	// get curent play time in ms
	size_t GetTime();
	// set curent play time in ms
	void SetTime(const size_t millisecond);

	inline bool IsPlay()
	{
		return AudioPlay;
	}
};
