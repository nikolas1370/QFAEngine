#include "pch.h"
#include "AudioLoader.h"
#include <Audio/Audio.h>
#include <mutex>
#include <Tools/String.h>
using std::cout;
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

#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

#define fourccRIFF 'FFIR'// Contains the letters "RIFF" in ASCII form (0x52494646 big-endian form).
#define fourccDATA 'atad' // all next big-endian
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd' // last big-endian

uint8_t QFAAudioLoader::Mp3Buffer[QFAAudioLoader::Mp3BufferSize];
QFAAudioLoader::~QFAAudioLoader()
{
    Delete();    
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
    MaxBuffersize = bufferSize; // in GetWavInfo or GetMp3Info MaxBuffersize can be changed
    if (OpenStatus status = GetWavInfo())
    {
        if (status == FileNotSupportNotRiff)
        {
            if (!GetMp3Info())
            {
                Status = FileNotSupport;
                return Status;
            }
        }
        else
        {
            Status = status;
            return Status;
        }
    }    

    AllocMemory();
    return Status = OK;
}

void QFAAudioLoader::Delete()
{
    if (!FileName.empty())
    {
        FileName.clear();

        if(AType == AudioType::Mp3)
            free((void*)Decls);
        else
            free((void*)Buffers[0].pAudioData);

        if (IsAudioStream)
            FileStream.Close();
    }
}


XAUDIO2_BUFFER& QFAAudioLoader::ReadFrom(size_t millisecond)
{

    if (MaxTime <= millisecond)
    {
        Buffers[2].AudioBytes = 0;
        StreamToEnd();
        return Buffers[2];
    }
    
    size_t startMs = (size_t)((double)NeedStartFrom * FrameTime);
    if (millisecond < startMs)
        millisecond = startMs;

    size_t endMs = (size_t)((double)EndReadFrame * FrameTime);
    if (millisecond >= endMs)
    {
        Buffers[2].AudioBytes = 0;
        return Buffers[2];
    }
    
    PlayStartFrom = (size_t)((double)millisecond / FrameTime);
    const UINT32 maxSkipFrame = 2000;
    UINT32 playBegin;
    if (PlayStartFrom > maxSkipFrame)
    {
        playBegin = maxSkipFrame;
        ReadFrames = PlayStartFrom - maxSkipFrame;
    }
    else
    {        
        ReadFrames = 0;
        playBegin = PlayStartFrom;
    }
    
    XAUDIO2_BUFFER& buff = Read();
    if (AType == AudioType::Mp3)
        buff.PlayBegin = playBegin;  // this is bug fix but it's lame.

    return buff;
}

XAUDIO2_BUFFER& QFAAudioLoader::Read()
{
    if (ReadFrames >= EndReadFrame)
    {
        Buffers[2].AudioBytes = 0;
        return Buffers[2];
    }

    XAUDIO2_BUFFER& buff = GetBuffer(ReadFrames);
    ReadFrames += buff.AudioBytes / FrameSize;    
    return buff;
}

void QFAAudioLoader::AllocMemory()
{
    size_t countRead = 0;
    FileStream.SetFilePointer(DataOffset);
    if (AType == AudioType::Wave)
    {
        if (IsAudioStream)
            Buffers[0].pAudioData = (BYTE*)malloc(MaxBuffersize);
        else
        {
            Buffers[0].pAudioData = (BYTE*)malloc(DataSize);
            FileStream.Read(DataSize, Buffers[0].pAudioData, countRead, DataOffset + DataSize);
            Buffers[0].AudioBytes = countRead;
            FileStream.Close();
        }
    }
    else
    {
        QFAFileReadStream stream;
        stream.Open(FileName);
        Mp3FileSize = stream.GetFileSize();
        if (IsAudioStream)
        {
            Decls = (mp3dec_t*)malloc(sizeof(*Decls) + MaxMp3Buffersize + MaxBuffersize);
            Buffers[0].pAudioData = (BYTE*)Decls + sizeof(*Decls);
            Buffers[1].pAudioData = Buffers[0].pAudioData + MaxMp3Buffersize;
        }
        else
        {
            Decls = (mp3dec_t*)malloc(sizeof(*Decls) + stream.GetFileSize() + MaxBuffersize);
            Buffers[0].pAudioData = (BYTE*)Decls + sizeof(*Decls);
            Buffers[1].pAudioData = Buffers[0].pAudioData + stream.GetFileSize();
            size_t count;
            stream.Read(stream.GetFileSize(), Buffers[0].pAudioData, count);
            stream.Close();
        }
    }
}

XAUDIO2_BUFFER& QFAAudioLoader::GetBuffer(size_t frame)
{
    if (AType == AudioType::Wave)
        return GetBufferWave(frame);
    else
    {
        if (IsAudioStream)
        {
            size_t ms = (size_t)((double)frame * FrameTime);           
            FileStream.SetFilePointer(FramesBlock[ms / (Mp3FrameTime * MaxAmountFrameInFramesBlock)].Offset);
            size_t countRead;
            FileStream.Read(MaxMp3Buffersize, Buffers[0].pAudioData, countRead, DataOffset + EndReadFrame * FrameSize);             
            return GetBufferMp3(frame, (char*)Buffers[0].pAudioData, FramesBlock[ms / (Mp3FrameTime * MaxAmountFrameInFramesBlock)].Offset);
        }
        else
            return GetBufferMp3(frame, (char*)Buffers[0].pAudioData, 0);
    }
}

XAUDIO2_BUFFER& QFAAudioLoader::GetBufferMp3(const size_t frame, const char* inputBuffer, size_t fileOffset)
{
    XAUDIO2_BUFFER& buf = Buffers[2];
    if (EndReadFrame <= frame)
    {
        buf.AudioBytes = 0;
        return buf;
    }

    size_t offset, outOffset = 0, notNeed;
    const int minimalRawBufferSize = CountFrameInMp3Frame * FrameSize;
    int needDecodeMp3Frame = MaxBuffersize / minimalRawBufferSize;

    const size_t remainedFrame = EndReadFrame - frame;
    bool needSetEnd = false;
    size_t tem = (size_t)ceil((double)remainedFrame / (double)CountFrameInMp3Frame);    
    if (tem < needDecodeMp3Frame)
    {
        needSetEnd = true;
        needDecodeMp3Frame = tem;
    }

    size_t countMp3FrameRead = frame / CountFrameInMp3Frame;  
    const size_t startCountMp3FrameRead = countMp3FrameRead;
    for (size_t i = 0; i < needDecodeMp3Frame; i++)
    {
        if (!FindMp3Frame(countMp3FrameRead, offset, notNeed) || offset >= Mp3FileSize)
            break;

        int mp3Size = offset + MaxMp3FrameSize * MaxAmountFrameInFramesBlock > Mp3FileSize ? Mp3FileSize - offset : MaxMp3FrameSize * MaxAmountFrameInFramesBlock;
        offset -= fileOffset;

        const int frameCount = DecodeMp3(inputBuffer + offset, mp3Size, Buffers[1].pAudioData + outOffset);
        countMp3FrameRead++;

        if (frameCount == CountFrameInMp3Frame)
            outOffset += minimalRawBufferSize;
        else
        {
            outOffset += frameCount * FrameSize;
            break;
        }
    }

    tem = ReadFrames + outOffset / FrameSize;
    if (needSetEnd || tem > EndReadFrame)
    {
        buf.Flags = XAUDIO2_END_OF_STREAM;
        buf.AudioBytes = outOffset - (tem - EndReadFrame) * FrameSize;
    }
    else
    {
        buf.Flags = 0;
        buf.AudioBytes = outOffset;
    }
    
    buf.PlayLength = 0;
    buf.PlayBegin = 0;
    buf.pAudioData = Buffers[1].pAudioData;
    return buf;
}

XAUDIO2_BUFFER& QFAAudioLoader::GetBufferWave(size_t frame)
{
    if (IsAudioStream)
    {        
        FileStream.SetFilePointer(DataOffset + frame * FrameSize);
        size_t countRead = 0;// read next buffer
        FileStream.Read(MaxBuffersize, Buffers[0].pAudioData, countRead, DataOffset + EndReadFrame * FrameSize);
        Buffers[0].AudioBytes = countRead;
        if (FileStream.GetFilePointer() - DataOffset >= EndReadFrame * FrameSize)
            Buffers[0].Flags = XAUDIO2_END_OF_STREAM;
        else
            Buffers[0].Flags = 0;

        return Buffers[0];
    }
    else
    {
        Buffers[1].PlayBegin = frame;
        Buffers[1].PlayLength = 0;
        Buffers[1].pAudioData = Buffers[0].pAudioData;
        Buffers[1].AudioBytes = EndReadFrame * FrameSize;
        Buffers[1].Flags = XAUDIO2_END_OF_STREAM;
        return Buffers[1];
    }
}

void QFAAudioLoader::StreamToEnd()
{
    if (IsAudioStream)
        FileStream.SetFilePointerToEnd();
    else
        ReadFrames = MAXUINT64;
}

void QFAAudioLoader::SetStartTime(size_t millisecond)
{
    size_t curentTime = ParentAudio->GetTime();
    if (millisecond >= EndReadFrame * FrameTime)
        return;

    NeedStartFrom = millisecond / FrameTime;    
    if (curentTime < millisecond)
    {
        ParentAudio->SetTime(millisecond);
    }
}

void QFAAudioLoader::SetEndTime(size_t millisecond)
{
    if (millisecond <= (size_t)((double)NeedStartFrom * FrameTime))
        return;

    if (millisecond > MaxTime)
        millisecond = MaxTime;

    EndReadFrame = millisecond / FrameTime;
    ParentAudio->SetTime(ParentAudio->GetTime());
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

    if (!fmtFound)
        return NotFoundFmtSubChunk;
    else if(!dataFound)
        return NotFoundDataSubChunk;

    if(DataSize >= Wfx.Format.nAvgBytesPerSec) // max time >= 1000ms
        MaxTime = (size_t)((double)DataSize / (double)Wfx.Format.nAvgBytesPerSec * 1000.0);
    else
        MaxTime = (size_t)((1.0 - ((double)Wfx.Format.nAvgBytesPerSec / (double)DataSize - 1)) * 1000.0);

    FrameTime = 1.0 / ((double)Wfx.Format.nSamplesPerSec / 1000.0);
    FrameSize = Wfx.Format.nChannels * (Wfx.Format.wBitsPerSample / 8);
    EndReadFrame = (size_t)((double)MaxTime / FrameTime);

    CountFrame = DataSize / FrameSize;

    AType = AudioType::Wave;

    MaxBuffersize = MaxBuffersize - MaxBuffersize % FrameSize; // round size of buffer to frame size
    if (MaxBuffersize > DataSize)
        MaxBuffersize = DataSize;

    return OK;
}

bool QFAAudioLoader::GetMp3Info()
{
    static std::mutex ReadMp3Mut;
    static mp3dec_ex_t Mp3Dec = {0};
    QFAFileReadStream stream;

    size_t read;
    stream.Open(FileName);
    ReadMp3Mut.lock();
    stream.Read(Mp3BufferSize, Mp3Buffer, read);
    mp3dec_ex_open_buf(&Mp3Dec, (uint8_t*)Mp3Buffer, Mp3BufferSize, 0);// need for dec.samples. if dec.samples not need use mp3dec_decode_frame
    ReadMp3Mut.unlock();
    if (Mp3Dec.samples / Mp3Dec.info.channels > Mp3Dec.info.hz)// max time >= 1000ms
        MaxTime = (size_t)(((double)Mp3Dec.samples / (double)Mp3Dec.info.channels / (double)Mp3Dec.info.hz) * 1000.0);
    else
        MaxTime = (size_t)((1.0 - (double)Mp3Dec.info.hz / ((double)Mp3Dec.samples / (double)Mp3Dec.info.channels) - 1) * 1000.0);
        
    FrameTime = 1.0 / ((double)Mp3Dec.info.hz / 1000.0);
    FrameSize = (int)(Mp3Dec.info.frame_bytes / (Mp3Dec.info.hz / 1000));
    CountFrame = Mp3Dec.samples / Mp3Dec.info.channels;
    EndReadFrame = MaxTime / FrameTime;

    Wfx.Format.wFormatTag = 1; // Signed data
    Wfx.Format.nChannels = Mp3Dec.info.channels;
    Wfx.Format.nSamplesPerSec = Mp3Dec.info.hz;
    Wfx.Format.nAvgBytesPerSec = Mp3Dec.info.hz * FrameSize;
    Wfx.Format.nBlockAlign = FrameSize;// byte per frame 
    Wfx.Format.wBitsPerSample = FrameSize / Mp3Dec.info.channels * 8;

    CountFrameInMp3Frame = hdr_frame_samples(Mp3Dec.mp3d.header);// count sample per channel // mp3dec_decode_frame also returns CountFrameInMp3Frame
    
    int minimalRawBufferSize = CountFrameInMp3Frame * FrameSize;
    if (minimalRawBufferSize >= MaxBuffersize) // round size of buffer to mp3 frame
    {
        MaxBuffersize = minimalRawBufferSize;
        MaxMp3Buffersize = MaxMp3FrameSize * MinMp3Frame;
    }
    else
    {
        MaxBuffersize -= MaxBuffersize % minimalRawBufferSize;
        MaxMp3Buffersize = MaxBuffersize / minimalRawBufferSize * MaxMp3FrameSize;
        if (MaxMp3Buffersize < MaxMp3FrameSize * MinMp3Frame)
            MaxMp3Buffersize = MaxMp3FrameSize * MinMp3Frame;
    }

    Mp3VbrFound = Mp3Dec.vbr_tag_found;
    double samples = Mp3Dec.samples;    
    double channels = Mp3Dec.info.channels;
    mp3dec_ex_close(&Mp3Dec);
    LoadMp3Time((int)ceil(samples / channels / (double)CountFrameInMp3Frame), &Mp3Dec.mp3d);
    AType = AudioType::Mp3;
    return true;
}

void QFAAudioLoader::LoadMp3Time(const int mp3FrameCount, void* _mp3d)
{
    mp3dec_t* mp3d = (mp3dec_t*)_mp3d;

    std::unique_ptr<char> file(new char[FileStream.GetFileSize()]);
    size_t frameBlockCount = (size_t)ceil((double)mp3FrameCount / (double)MaxAmountFrameInFramesBlock);
    FramesBlock.reserve(frameBlockCount);
    FileStream.SetFilePointer(0);
    size_t read;
    const uint8_t* mp3 = (uint8_t*)file.get();
    FileStream.Read(FileStream.GetFileSize(), mp3, read);
    Mp3FramesBlock mfb;
    mfb.Millisecond = 0;
    int totalFrameCount = 0;
    int frameCount = 0;//  frame count in block
    int blockSize = 0, lastBlockSize = 0;
    int frame_bytes;    
    int offset = 0;
    if (Mp3VbrFound) // skip vbr
        mfb.Offset = offset = mp3d_find_frame(mp3, read, &mp3d->free_format_bytes, &frame_bytes) + frame_bytes;
    else
        mfb.Offset =  mp3d_find_frame(mp3, read, &mp3d->free_format_bytes, &frame_bytes);

    for (; offset < read && totalFrameCount < mp3FrameCount; totalFrameCount++, frameCount++)
    {
        if (frameCount == MaxAmountFrameInFramesBlock)
        {            
            if (FramesBlock.size())
            {
                mfb.Offset = FramesBlock.back().Offset + lastBlockSize;
                mfb.Millisecond = FramesBlock.back().Millisecond + (int)floor((double)CountFrameInMp3Frame * FrameTime) * MaxAmountFrameInFramesBlock;
            }

            lastBlockSize = blockSize;
            blockSize = 0;
            frameCount = 0;
            FramesBlock.push_back(mfb);
        }
        
        memset(mp3d, 0, sizeof(mp3dec_t));        
        int frame_offset = mp3d_find_frame(mp3 + offset, read - offset, &mp3d->free_format_bytes, &frame_bytes);
        offset += frame_offset + frame_bytes;
        blockSize += frame_bytes;
        mfb.FramesSize[frameCount] = frame_bytes;
    }

    if (frameCount > 0)
    {
        mfb.Offset = FramesBlock.back().Offset + lastBlockSize;
        mfb.Millisecond = FramesBlock.back().Millisecond + (int)floor((double)CountFrameInMp3Frame * FrameTime) * MaxAmountFrameInFramesBlock;
        LastBlockSize = frameCount;
        for (size_t i = frameCount; i < MaxAmountFrameInFramesBlock; i++)
            mfb.FramesSize[i] = 0;

        FramesBlock.push_back(mfb);
    }

    Mp3FrameTime = (double)CountFrameInMp3Frame * FrameTime;
    CountMp3Frame = ((int)FramesBlock.size() - 1) * MaxAmountFrameInFramesBlock + LastBlockSize;
}

bool QFAAudioLoader::FindMp3Frame(const int index, size_t& offset, size_t& millisecond)
{
    if (CountMp3Frame <= index || index < 0)
        return false;

    const int block = index / MaxAmountFrameInFramesBlock;
    const int start = block * MaxAmountFrameInFramesBlock;
    const int end = index - start;
    offset = FramesBlock[block].Offset;
    millisecond = FramesBlock[block].Millisecond;
    for (int i = 0; i < end; i++)
        offset += FramesBlock[block].FramesSize[i];
    
    return true;
}

int QFAAudioLoader::DecodeMp3(const void* mp3, const int mp3Bytes, const void* outFrames)
{
    mp3dec_frame_info_t info;
    static std::mutex decodeMp3Mut;
    decodeMp3Mut.lock();
    int suc = mp3dec_decode_frame(Decls, (uint8_t*)mp3, mp3Bytes, (mp3d_sample_t*)outFrames, &info);
    if (suc == 0) // some time mp3dec_decode_frame can not work need try twice
    {
        suc = mp3dec_decode_frame(Decls, (uint8_t*)mp3, mp3Bytes, (mp3d_sample_t*)outFrames, &info);
        if (suc == 0)
            suc = hdr_frame_samples(Decls->header);
    }

    decodeMp3Mut.unlock();
    return suc;
}
