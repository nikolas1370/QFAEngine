#include "pch.h"
#include "Audio.h"
IXAudio2* QFAAudio::XAudio2 = nullptr;
IXAudio2MasteringVoice* QFAAudio::XAudio2MasteringVoice = nullptr;
using std::cout;

QFAAudio::QFAAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{    
    SetAudio(fileName, isAudioStream, bufferSize);

}

QFAAudio::QFAAudio()
{

}

QFAAudio::~QFAAudio()
{
    if (XAudio2SourceVoice)
        XAudio2SourceVoice->DestroyVoice();
}

void QFAAudio::SetAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{
    Loader.Delete();
    Loader.ParentAudio = this;
    // CoInitializeEx(nullptr, COINIT_MULTITHREADED); // First, you need to have initialized COM. If you're using C++/WinRT, then it's taken care of. If you're not certain that your environment has already initialized COM, then you can call CoInitializeEx as long as you check the return value.
    // if use CoInitializeEx not forget use CoUninitialize();
    if (QFAAudioLoader::OpenStatus stat = Loader.OpenFile(fileName, isAudioStream, bufferSize))
        stopExecute(stat);

    Init();
    HRESULT hr = XAudio2->CreateSourceVoice(&XAudio2SourceVoice, (WAVEFORMATEX*)&Loader.Wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &VoiceCallback);
    VoiceCallback.QAudio = this;
    if (FAILED(hr))
        stopExecute(hr);
}

void QFAAudio::Init()
{
    if (!XAudio2) // init QFAAudio
    {
        HRESULT hr = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR); // To create an instance of the XAudio2 engine, call the XAudio2Create function. That will give you a pointer to an IXAudio2 interface, and it's a good idea to store that in a class data member. In this snippet we're using a C++/WinRT smart pointer, but you could use a raw pointer if necessary.
        if (FAILED(hr))
            stopExecute("");

        // Next, to create what's known as a mastering voice, call the IXAudio2::CreateMasteringVoice method. That will give you a pointer to an IXAudio2MasteringVoice interface. A mastering voices encapsulates an audio device. It's the ultimate destination for all audio that passes through an audio graph.
        XAudio2->CreateMasteringVoice(&XAudio2MasteringVoice);
    }
}

DWORD QFAAudio::GetChannelMask()
{
    DWORD dwChannelMask;
    XAudio2MasteringVoice->GetChannelMask(&dwChannelMask);
    
    return dwChannelMask;
}

void QFAAudio::SetVolume(const float volume) 
{
    XAudio2SourceVoice->SetVolume(volume);
}

void QFAAudio::RecreateVoice()
{
    XAudio2SourceVoice->DestroyVoice();
    HRESULT hr = XAudio2->CreateSourceVoice(&XAudio2SourceVoice, (WAVEFORMATEX*)&Loader.Wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &VoiceCallback);
    if (FAILED(hr))
        stopExecute("");
}

size_t QFAAudio::GetTime()
{
    XAUDIO2_VOICE_STATE pVoiceState;
    XAudio2SourceVoice->GetState(&pVoiceState);
    return (size_t)((double)(pVoiceState.SamplesPlayed + Loader.PlayStartFrom) * Loader.FrameTime);
}

void QFAAudio::Stop()
{
    if (!XAudio2SourceVoice)
        return;

    HRESULT hr = XAudio2SourceVoice->Stop(0);
    if (FAILED(hr))
        stopExecute("");

    AudioPlay = false;
}

void QFAAudio::Play()
{
    if (AudioPlay)
        return;

    XAUDIO2_VOICE_STATE pVoiceState;
    XAudio2SourceVoice->GetState(&pVoiceState);
    if (!pVoiceState.BuffersQueued)
    {
        XAUDIO2_BUFFER& buffer = ReadBuffer();
        if (!buffer.AudioBytes)
            return;
                
        HRESULT hr = XAudio2SourceVoice->SubmitSourceBuffer(&buffer);
        if (FAILED(hr))
            stopExecute("");
    }
    
    HRESULT hr = XAudio2SourceVoice->Start(0);
    if (FAILED(hr))
        stopExecute("");

    AudioPlay = true;
}

void QFAAudio::SetTime(size_t millisecond)
{
    if (millisecond >= Loader.EndReadFrame * Loader.FrameTime)
    {
        if (Repeat)
            millisecond = Loader.NeedStartFrom * Loader.FrameTime;
        else
            return;
    }

    XAUDIO2_VOICE_STATE pVoiceState;
    XAudio2SourceVoice->GetState(&pVoiceState);
    if (pVoiceState.BuffersQueued)
    {
        RecreateVoice();
        VoiceCallback.QAudio = this;
        if (AudioPlay)
        {
            HRESULT hr = XAudio2SourceVoice->Start(0);
            if (FAILED(hr))
                stopExecute("");
        }
    }

    XAUDIO2_BUFFER& buffer = ReadBuffer(false, millisecond);
    if (!buffer.AudioBytes)
        return;
         
    HRESULT hr = XAudio2SourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr))
        stopExecute("");
}

XAUDIO2_BUFFER& QFAAudio::ReadBuffer(bool next, size_t millisecond)
{    
    XAUDIO2_BUFFER& buffer  = next ? Loader.Read() : Loader.ReadFrom(millisecond);
    if (!buffer.AudioBytes)
    {
        if (Repeat)
            buffer = Loader.ReadFrom(0);
        else 
        {
            AudioPlay = false;
            XAudio2SourceVoice->Stop();
        }
    }

    return buffer;
}

void QFAAudio::BufferEnd()
{
    AudioPlay = false;
    Play();
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
