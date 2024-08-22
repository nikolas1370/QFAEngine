#include "pch.h"
#include "Audio.h"
#include <xaudio2.h>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
IXAudio2* QFAAudio::XAudio2 = nullptr;
IXAudio2MasteringVoice* QFAAudio::XAudio2MasteringVoice = nullptr;

QFAAudio::QFAAudio(const wchar_t* fileName)
{// https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--play-a-sound-with-xaudio2
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

    WAVEFORMATEXTENSIBLE wfx = { 0 };
    XAUDIO2_BUFFER xAudio2Buffer = { 0 };

    // Open the file
    HANDLE hFile = CreateFile(
        fileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        stopExecute("");

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        stopExecute("");

    // 3 Locate the 'RIFF' chunk in the audio file, and check the file type.
    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE)
        stopExecute("");

    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    DataBuffer = new BYTE[dwChunkSize];
    ReadChunkData(hFile, DataBuffer, dwChunkSize, dwChunkPosition);

    xAudio2Buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    xAudio2Buffer.pAudioData = DataBuffer;  //buffer containing audio data
    xAudio2Buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

    //Next, to create what's known as a source voice, call the IXAudio2::CreateSourceVoice method. That will give you a pointer to an IXAudio2SourceVoice interface. The format of the voice is specified by the values set in the WAVEFORMATEX structure.
    hr = XAudio2->CreateSourceVoice(&XAudio2SourceVoice, (WAVEFORMATEX*)&wfx);
    if (FAILED(hr))
        stopExecute("");    

    // Submit an XAUDIO2_BUFFER to the source voice by calling the IXAudio2SourceVoice::SubmitSourceBuffer method.
    hr = XAudio2SourceVoice->SubmitSourceBuffer(&xAudio2Buffer);
    if (FAILED(hr))
        stopExecute("");
}

QFAAudio::~QFAAudio()
{
    if(XAudio2SourceVoice)
    {
        XAudio2SourceVoice->DestroyVoice();
        delete[] DataBuffer;
    }
}

void QFAAudio::Play()
{
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

HRESULT QFAAudio::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;
}

HRESULT QFAAudio::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}