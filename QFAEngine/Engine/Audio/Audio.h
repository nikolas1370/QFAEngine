#pragma once
#include <Tools/Stuff.h>
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

class QFAEXPORT QFAAudio
{
	const wchar_t* FileName = nullptr;
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

	BYTE* DataBuffer;

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
public:
	QFAAudio(const wchar_t* fileName);
	~QFAAudio();

	void Play();
	void Stop();


};
