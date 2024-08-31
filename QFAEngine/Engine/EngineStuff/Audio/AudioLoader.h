#pragma once
#include <xaudio2.h>
#include <Tools/File/FileSystem.h>
class QFAAudio;
class QFAAudioLoader // only for QFAAudio
{// load and extract audio
	friend QFAAudio;

	enum OpenStatus
	{
		OK = 0, // file open and processed
		NotOpen,	
		PathEmpty,
		FileNotExis, 
		PathNotFile,
		FileSizeTooSmall,
		NotFoundFmtSubChunk,
		NotFoundDataSubChunk,
		FileNotSupportNotRiff, 
		FileNotSupportNotWave
	};

	OpenStatus Status = OpenStatus::NotOpen;
	std::u32string FileName;
	int MaxBuffersize; // this be size in XAUDIO2_BUFFER::AudioBytes
	WAVEFORMATEXTENSIBLE Wfx;
	/*
	    if IsAudioStream == true use only first buffer
		if IsAudioStream == false first buffer use like storage for 
			allocated memory second is temporary buffer
	*/
	XAUDIO2_BUFFER Buffers[2] = {0};
	QFAFileReadStream FileStream;
	bool IsAudioStream;
	size_t DataSize;
	size_t DataOffset; // offset from start file; use if Is
	size_t MaxTime = 0;// in millisecond
	double FrameTime = 0.0; // frame duration in millisecond	
	unsigned char FrameSize;// in 2 channel with sample 2 byte be Frame be 4 bytes
	size_t CountFrame;
	
	size_t CountBytesRead = 0; // only if IsAudioStream == false

	QFAAudioLoader() {}
	~QFAAudioLoader();	
	OpenStatus OpenFile(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize);
	OpenStatus GetWavInfo();

	/*
	read next data in file

	if XAUDIO2_BUFFER.AudioBytes == 0 audio end
	if XAUDIO2_BUFFER.Flags == XAUDIO2_END_OF_STREAM this buffer last
	*/
	XAUDIO2_BUFFER& Read();
	/*
	read audio file from some ms

	if XAUDIO2_BUFFER.AudioBytes == 0 audio end
	if XAUDIO2_BUFFER.Flags == XAUDIO2_END_OF_STREAM this buffer last 
	*/
	XAUDIO2_BUFFER& ReadFrom(size_t millisecond);
	

	void StreamToEnd();
};