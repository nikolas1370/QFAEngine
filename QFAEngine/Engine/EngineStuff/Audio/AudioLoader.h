#pragma once
#include <xaudio2.h>
#include <Tools/File/FileSystem.h>
#include <vector>
#include <array>
class QFAAudio;
class QFAAudioLoader // only for QFAAudio
{// load and extract audio
	/*
		amount samples in frame equal numper of channels
		mp3Frame != frame
	*/
	friend QFAAudio;
	static const unsigned char MaxAmountFrameInFramesBlock = 10;
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
		FileNotSupportNotWave,
		FileNotSupport
	};

	enum AudioType
	{
		UndefinedAudioType,
		Wave,
		Mp3
	};

	struct Mp3FramesBlock
	{
		unsigned int Millisecond; // first frame start in millisecond from audio start
		unsigned int Offset; // offset from start mp3 file to first frame in block
		std::array<unsigned short, MaxAmountFrameInFramesBlock> FramesSize; // in Byte in mp3 file		
	};

	static const int Tag1Size = 128;// metadata in mp3 store in end file	
	static const int Mp3BufferSize = 16 * 1024;// minimal 10 mp3 frame
	static const int MinMp3Frame = 10; // min mp3 frame for mp3dec_decode_frame 
	static const int MaxMp3FrameSize = Mp3BufferSize / MinMp3Frame;
	/*
	
	need mutex if use Mp3Buffer
	*/
	static uint8_t Mp3Buffer[Mp3BufferSize]; //store part of mp3 file	
	std::vector<Mp3FramesBlock> FramesBlock;
	unsigned char LastBlockSize = 0; // last block(in FramesBlock) have this number mp3Frame
	int CountFrameInMp3Frame = 0; // use in mp3


	AudioType AType = UndefinedAudioType;

	QFAAudio* ParentAudio;

	OpenStatus Status = OpenStatus::NotOpen;
	std::u32string FileName;
	int MaxBuffersize;		// amount byte for store raw audio data
	int MaxMp3Buffersize;   // amount byte for store mp3 data
	WAVEFORMATEXTENSIBLE Wfx;
	/*
		wave
			if IsAudioStream == true use only first buffer
			if IsAudioStream == false first buffer store all wavw audio data.
				second is temporary buffer
		mp3
			if IsAudioStream == true in first buffer store part of mp3 file
				second store allocated memory for decoded audio.
				third is temporary buffer
			if IsAudioStream == false in first buffer store whole mp3 file
			   second store allocated memory for decoded audio.
			   third is temporary buffer
	*/
	XAUDIO2_BUFFER Buffers[3] = {0};
	
	QFAFileReadStream FileStream;
	bool IsAudioStream;
	/* use in wave */
	size_t DataSize; 
	size_t DataOffset; // offset from start file; 
	/*-------------*/
	size_t MaxTime = 0;// in millisecond 
	double FrameTime = 0.0; // frame duration in millisecond	
	double Mp3FrameTime = 0.0; // mp3Frame duration in millisecond
	unsigned char FrameSize;// in 2 channel with sample 2 byte be Frame be 4 bytes
	size_t CountFrame;
	size_t CountMp3Frame;
	size_t Mp3FileSize = 0; 
	


	/*
	// next 4 delete
	size_t StartReadFrom = 0; // in ms
	size_t EndRead = 0;       // in ms
	size_t EndReadByte = 0;   // in byte	wave
	size_t CountMp3FrameRead = 0;
	*/



	size_t NeedStartFrom = 0; // in frame
	size_t PlayStartFrom = 0; // in frame. for getTime
	size_t ReadFrames = 0;	  // from start audio
	size_t EndReadFrame = 0;

	bool Mp3VbrFound = false; // if false mp3 CBR
	QFAAudioLoader() {}
	~QFAAudioLoader();	
	OpenStatus OpenFile(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize);
	void AllocMemory();
	OpenStatus GetWavInfo();
	bool GetMp3Info();
	// mp3dec_t* mp3d
	void LoadMp3Time(const int mp3FrameCount, void* mp3d);

	
	/*
		index of mp3Frame
		millisecond return when start frameBlock in ms
		return true if mp3Frame found
	*/
	bool FindMp3Frame(const int index, size_t& offset, size_t& millisecond);
	

	/*
		mp3 pointer at part mp3 file
		mp3Bytes size of mp3 data
		outFrames write decode data
		return count frame was decode
	*/
	int DecodeMp3(const void* mp3, const int mp3Bytes, const void* outFrames);

	XAUDIO2_BUFFER& GetBuffer(size_t frame);
	XAUDIO2_BUFFER& GetBufferWave(size_t frame);
	XAUDIO2_BUFFER& GetBufferMp3(const size_t frame, const char* inputBuffer, size_t fileOffset);
public:

	/*
	read next data in file

	if XAUDIO2_BUFFER.AudioBytes == 0 audio end
	if XAUDIO2_BUFFER.Flags == XAUDIO2_END_OF_STREAM this buffer last
	*/
	XAUDIO2_BUFFER& Read();
	/*
	read audio file from some ms 
	if ms less than startTime be play from startTime
	//if ms > endTime be play from startTime

	*/
	XAUDIO2_BUFFER& ReadFrom(size_t millisecond);
	

	/*
	rework for mp3
	
	*/
	void StreamToEnd();

	// return true if curentTime < millisecond
	void SetStartTime(size_t millisecond);
	void SetEndTime(size_t millisecond);

};