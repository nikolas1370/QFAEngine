#include "pch.h"
#include "AudioLoader.h"
#include <Tools/String.h>
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

#define fourccRIFF 'FFIR'// Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
#define fourccDATA 'atad' // all next big-endian
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd' // last big-endian

QFAAudioLoader::~QFAAudioLoader()
{
    if (!FileName.empty())
    {
        delete Buffers[0].pAudioData;        
        if(IsAudioStream)
            FileStream.Close();
    }
}

QFAAudioLoader::OpenStatus QFAAudioLoader::OpenFile(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{
    if (fileName.empty())
    {
        Status = PathEmpty;
        return PathEmpty;
    }

    std::filesystem::path path(fileName);
    if (!std::filesystem::exists(path))
    {
        Status = FileNotExis;
        return FileNotExis;
    }

    if (std::filesystem::is_directory(path))
    {
        Status = PathNotFile;
        return PathNotFile;
    }

    FileName = fileName;
    IsAudioStream = isAudioStream;    
    if (OpenStatus status = GetWavInfo())
    {
        Status = status;
        return Status;
    }    

    MaxBuffersize = bufferSize - bufferSize % FrameSize; // round size of buffer to frame size
    if (MaxBuffersize > DataSize)
        MaxBuffersize = DataSize;

    size_t countRead = 0;
    FileStream.SetFilePointer(DataOffset);
    if (IsAudioStream)
        Buffers[0].pAudioData = new BYTE[MaxBuffersize];
    else
    {
        Buffers[0].pAudioData = new BYTE[DataSize];
        FileStream.Read(DataSize, Buffers[0].pAudioData, countRead, DataOffset + DataSize);
        Buffers[0].AudioBytes = countRead;
        FileStream.Close();
    }

    Status = OK;
    return Status;
}

XAUDIO2_BUFFER& QFAAudioLoader::Read()
{
    if (IsAudioStream)
    {
        size_t countRead = 0;// read next buffer
        FileStream.Read(MaxBuffersize, Buffers[0].pAudioData, countRead, DataOffset + DataSize);
        Buffers[0].AudioBytes = countRead;
        if (FileStream.GetFilePointer() - DataOffset >= DataSize)
            Buffers[0].Flags = XAUDIO2_END_OF_STREAM;
        else
            Buffers[0].Flags = 0;

        return Buffers[0];
    }
    else
    {
        Buffers[1].pAudioData = Buffers[0].pAudioData + CountBytesRead;
        if(DataSize - CountBytesRead < MaxBuffersize)
        {
            Buffers[1].AudioBytes = DataSize - CountBytesRead;
            Buffers[1].Flags = XAUDIO2_END_OF_STREAM;
        }
        else
        {
            Buffers[1].AudioBytes = MaxBuffersize;
            Buffers[1].Flags = 0;
        }

        CountBytesRead += MaxBuffersize;
        return Buffers[1];
    }
}

XAUDIO2_BUFFER& QFAAudioLoader::ReadFrom(size_t millisecond)
{
    if (IsAudioStream)
    {
        size_t countRead = 0;
        FileStream.SetFilePointer(DataOffset + (size_t)((double)millisecond / FrameTime) * FrameSize);
        FileStream.Read(MaxBuffersize, Buffers[0].pAudioData, countRead, DataOffset + DataSize);
        Buffers[0].AudioBytes = countRead;        
        if (FileStream.GetFilePointer() - DataOffset >= DataSize)
            Buffers[0].Flags = XAUDIO2_END_OF_STREAM;
        else
            Buffers[0].Flags = 0;        

        return Buffers[0];
    }
    else
    {
        CountBytesRead = (size_t)((double)millisecond / FrameTime) * FrameSize;        
        Buffers[1].pAudioData = Buffers[0].pAudioData + CountBytesRead;
        if (DataSize - CountBytesRead < MaxBuffersize)
        {
            Buffers[1].AudioBytes = DataSize - CountBytesRead;
            Buffers[1].Flags = XAUDIO2_END_OF_STREAM;
        }
        else
        {
            Buffers[1].AudioBytes = MaxBuffersize;
            Buffers[1].Flags = 0;
        }

        CountBytesRead += MaxBuffersize;
        return Buffers[1];
    }
}

void QFAAudioLoader::StreamToEnd()
{
    if (IsAudioStream)
        FileStream.SetFilePointerToEnd();
    else
        CountBytesRead = DataSize;
}

QFAAudioLoader::OpenStatus QFAAudioLoader::GetWavInfo()
{   // https://learn.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-?redirectedfrom=MSDN
    // http://soundfile.sapp.org/doc/WaveFormat/    
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

    char data[50];
    size_t ss;
    SWavHead wav;
    FileStream.Read(sizeof(SWavHead), &wav, ss);

    if (wav.ChunkId != fourccRIFF)
        return FileNotSupportNotRiff;

    if (wav.Format != fourccWAVE)
        return FileNotSupportNotWave;

    if (wav.ChunkSize >= FileStream.GetFileSize())
        return FileSizeTooSmall;

    size_t offset = sizeof(SWavHead); // offset from start file, move to first subchunk
    bool fmtFound = false;
    bool dataFound = false;
    for (; offset < wav.ChunkSize - sizeof(chunkId_t);)
    {
        FileStream.SetFilePointer(offset);
        FileStream.Read(sizeof(data), &data, ss);
        subChunkId_t subChunkId = *(subChunkId_t*)&data;
        if (subChunkId == fourccDATA)
        {// in DataChunkSize not include SubchunkId and SubchunkIdSize                        
            DataSize =  *(UINT32*)&data[sizeof(subChunkId_t)];
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

    if (!fmtFound)
        return NotFoundFmtSubChunk;
    else if(!dataFound)
        return NotFoundDataSubChunk;

    MaxTime = (size_t)((double)DataSize / (double)Wfx.Format.nAvgBytesPerSec * 1000.0);
    FrameTime = 1.0 / ((double)Wfx.Format.nSamplesPerSec / 1000.0);
    FrameSize = Wfx.Format.nChannels * (Wfx.Format.wBitsPerSample / 8);
    CountFrame = DataSize / FrameSize;
    return OK;
}