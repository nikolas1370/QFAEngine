#include "pch.h"
#include "Audio.h"

#define fourccRIFF 'FFIR'// Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
#define fourccDATA 'atad' // all next big-endian
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW' 
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd' // last big-endian
IXAudio2* QFAAudio::XAudio2 = nullptr;
IXAudio2MasteringVoice* QFAAudio::XAudio2MasteringVoice = nullptr;

/*
struct WavFileHeader {         //       Offset  Size  Type       Description
// ----------------------------------------------------------------------------------------------------------------
unsigned             RiffChunk;      // 0       4     FourCC     'RIFF'
unsigned             ChunkSize;      // 4       4     DWord      size of the riff chunk (should be always filesize - 8)
unsigned             FileFormat;     // 8       4     FourCC     'WAVE'
unsigned             FormatChunk;    // 12      4     FourCC     'fmt '
unsigned             FormatSize;     // 16      4     DWord      size of Format structure (should be always 16 byte)
unsigned short       PcmFlags;       // 20      2     Word       bit 1: Signed data, bit 2: Float data..
unsigned short       Channels;       // 22      2     Word       samples per frame (example: one stereo frame consist from 2 samples)
unsigned             SampleRate;     // 24      4     DWord      frames per second (example: 44100 stereo-frames are played back per seccond)
unsigned             ByteRate;       // 28      4     DWord      bytes per second (example: one second float32 stereo-track data: 44100frames * 2channels * 4bytes  )
unsigned short       BlockAlign;     // 32      2     Word       byte per frame (example: each float32 stero frame is 8 byte in size - one float is 4byte - 2 channels are 2 floats, each 4byte)
unsigned short       BitDepth;       // 34      2     Word       bits per sample (example: one float32 is 4 byte where each byte has 8 bit... so: 32 bit per sample )  
unsigned             DataChunk;      // 36      4     FourCC     'data'
unsigned             DataSize;       // 40      4     DWord      size of of payload data (should be the total file size minus this headers size of 44 byte)
}; 
    Notably: Microsoft's RIFF specification says that subchunks can appear in any order.
*/

QFAAudio::QFAAudio(const std::u32string& fileName, bool isAudioStream, size_t bufferSize)
{
    MaxBuffersize = bufferSize;
    IsAudioStream = isAudioStream;
	FileName = fileName;        
    // CoInitializeEx(nullptr, COINIT_MULTITHREADED); // First, you need to have initialized COM. If you're using C++/WinRT, then it's taken care of. If you're not certain that your environment has already initialized COM, then you can call CoInitializeEx as long as you check the return value.
    // if use CoInitializeEx not forget use CoUninitialize();
    HRESULT hr;

    if(!XAudio2) // init QFAAudio
    {
        hr = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR); // To create an instance of the XAudio2 engine, call the XAudio2Create function. That will give you a pointer to an IXAudio2 interface, and it's a good idea to store that in a class data member. In this snippet we're using a C++/WinRT smart pointer, but you could use a raw pointer if necessary.
        if (FAILED(hr))
            stopExecute("");

        // Next, to create what's known as a mastering voice, call the IXAudio2::CreateMasteringVoice method. That will give you a pointer to an IXAudio2MasteringVoice interface. A mastering voices encapsulates an audio device. It's the ultimate destination for all audio that passes through an audio graph.
        XAudio2->CreateMasteringVoice(&XAudio2MasteringVoice);
    }
    
    if (!GetWavInfo())
        stopExecute("");

    hr = XAudio2->CreateSourceVoice(&XAudio2SourceVoice, (WAVEFORMATEX*)&Wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &VoiceCallback);
    VoiceCallback.QAudio = this;
    if (FAILED(hr))
        stopExecute("");

    if (IsAudioStream)
    {
        Buffers[0].pAudioData = new BYTE[MaxBuffersize];
        Buffers[1].pAudioData = new BYTE[MaxBuffersize];        
    }
}

QFAAudio::~QFAAudio()
{
    if(XAudio2SourceVoice)
    {
        XAudio2SourceVoice->DestroyVoice();
        if (IsAudioStream)
        {
            delete Buffers[0].pAudioData;
            delete Buffers[1].pAudioData;
        }
    }
}

void QFAAudio::Play()
{
    XAUDIO2_VOICE_STATE pVoiceState;
    XAudio2SourceVoice->GetState(&pVoiceState);
    if (pVoiceState.BuffersQueued == 0)
    {
        if (IsAudioStream)
        {
            FileStream.SetFilePointer(DataOffset);
            LoadStreamFile(Buffers[0]);
            LoadStreamFile(Buffers[1]);
        }
        else
            LoadWholeFile();
    }

    HRESULT hr = XAudio2SourceVoice->Start(0);
    if (FAILED(hr))
        stopExecute("");
}

void QFAAudio::Stop()
{
    HRESULT hr = XAudio2SourceVoice->Stop(0);
    if (FAILED(hr))
        stopExecute("");
}

void QFAAudio::SetVolume(const float volume)
{
    XAudio2SourceVoice->SetVolume(volume);
}

bool QFAAudio::GetWavInfo()
{   // https://learn.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-?redirectedfrom=MSDN
    // http://soundfile.sapp.org/doc/WaveFormat/    
    if (QFAFileSystem::LoadFile(FileName, &FIle))
        stopExecute("cannot load");

    typedef unsigned chunkId_t;
    typedef unsigned subChunkId_t;
    struct SWavHead
    {
        chunkId_t ChunkId;
        unsigned ChunkSize;
        unsigned Format;
    };

    struct SSubChunkHead
    {
        subChunkId_t subChunkId;
        unsigned subChunkSize;
    };

    FileStream.Open(FileName);
    
    //char data[50] = {0};
    char data[50];
    size_t ss;
    SWavHead wav;
    FileStream.Read(sizeof(SWavHead), &wav, ss);    
    
    if (wav.ChunkId != fourccRIFF)
        stopExecute("Not Riff file");

    if (wav.Format != fourccWAVE)
        stopExecute("not wave file");

    size_t offset = sizeof(SWavHead); // offset from start file, move to first subchunk

    if (wav.ChunkSize < sizeof(chunkId_t))
        stopExecute("");

    bool fmtFound = false;
    bool dataFound = false;
    for (; offset < wav.ChunkSize - sizeof(chunkId_t);)
    {
        FileStream.SetFilePointer(offset);
        FileStream.Read(sizeof(data), &data, ss);
        subChunkId_t subChunkId = *(subChunkId_t*)&data;
        if (subChunkId == fourccDATA)
        {// in DataChunkSize not include SubchunkId and SubchunkIdSize                        
            DataSize = *(UINT32*)&data[sizeof(subChunkId_t)];
            DataOffset = offset + sizeof(SSubChunkHead);
            offset += DataSize + sizeof(SSubChunkHead);
            dataFound = true;
        }
        else if (subChunkId == fourccFMT)
        {
            Wfx.Format = *(WAVEFORMATEX*)&data[sizeof(SSubChunkHead)];
            offset += 16 + sizeof(SSubChunkHead);// Format structure should be always 16 byte
            fmtFound = true;
        }
        else // std::cout << "Subchunk Id " << std::string((char*)&subChunkId, 4) << " not support. Size of chunk " << *(unsigned*)&data[sizeof(subChunkId_t)] << "\n";
            offset += (*(unsigned*)&data[sizeof(subChunkId_t)]) + sizeof(SSubChunkHead); // subchunk size + SSubChunkHead
    }

    if (!fmtFound || !dataFound)
        stopExecute("not fount some");
    
    return true;
}

void QFAAudio::LoadStreamFile(XAUDIO2_BUFFER& buffer)
{
    size_t countRead = 0;
    FileStream.Read(MaxBuffersize, buffer.pAudioData, countRead, DataOffset + DataSize);
    if (countRead == 0)
    {
        if (Repeat)
        {
            FillBuffer(buffer, 0);
            buffer.AudioBytes = MaxBuffersize;
        }
        else
            return;
    }
    else if(MaxBuffersize != countRead)
    {
        if (Repeat)
        {
            FillBuffer(buffer, countRead);
            buffer.AudioBytes = MaxBuffersize;
        }
        else 
        {
            buffer.Flags = XAUDIO2_END_OF_STREAM;
            buffer.AudioBytes = countRead; 
        }
    }    
    else 
    { 
        if (!Repeat && FileStream.GetFilePointer() - DataOffset >= DataSize) // if reach end of audio data       
            buffer.Flags = XAUDIO2_END_OF_STREAM;

        buffer.AudioBytes = MaxBuffersize;
    }

    HRESULT hr = XAudio2SourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
        stopExecute("");
}

void QFAAudio::FillBuffer(XAUDIO2_BUFFER& buffer, size_t amountDataInbuffer)
{
    size_t amountDataInBuffer = amountDataInbuffer;
    size_t countRead = 0;
    while (true) // if audio data size less than MaxBuffersize
    {// loop work until whole buffer be filled
        if (amountDataInBuffer >= MaxBuffersize) 
            break;

        FileStream.SetFilePointer(DataOffset);
        FileStream.Read(MaxBuffersize - amountDataInBuffer, buffer.pAudioData + amountDataInBuffer, countRead, DataOffset + DataSize);
        amountDataInBuffer += countRead;
    }
}

void QFAAudio::BufferEnd()
{    
    if(IsAudioStream)
    {
        LoadStreamFile(UseFirstBuffer ? Buffers[0] : Buffers[1]);
        UseFirstBuffer = !UseFirstBuffer;
    }
    else if (Repeat)
    {   
        HRESULT hr = XAudio2SourceVoice->SubmitSourceBuffer(&Buffers[0]);
        if (FAILED(hr))
            stopExecute("");
    }
}

void QFAAudio::LoadWholeFile()
{
    QFAFileSystem::LoadFile(FileName, &FIle);
    Buffers[0].AudioBytes = DataSize;
    Buffers[0].pAudioData = ((BYTE*)FIle.GetData()) + DataOffset;
    Buffers[0].Flags = XAUDIO2_END_OF_STREAM;
    HRESULT hr = XAudio2SourceVoice->SubmitSourceBuffer(&Buffers[0]);
    if (FAILED(hr))
        stopExecute("");
}

QFAAudio::VoiceCallback::VoiceCallback()
{
    hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

QFAAudio::VoiceCallback::~VoiceCallback()
{
    CloseHandle(hBufferEndEvent);
}

void QFAAudio::VoiceCallback::OnBufferEnd(void* pBufferContext)
{    
    QAudio->BufferEnd();
}
