#include "pch.h"
#include "Audio3D.h"

X3DAUDIO_HANDLE QFAAudio3D::X3DInstance = {};
bool QFAAudio3D::IsInit = false;

QFAAudio3D::QFAAudio3D()
{
}
QFAAudio3D::QFAAudio3D(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{
	SetAudio(fileName, isAudioStream, bufferSize);
}

QFAAudio3D::~QFAAudio3D()
{

}

void QFAAudio3D::Init()
{
	if (!IsInit)
	{
		DWORD dwChannelMask = QFAAudio::GetChannelMask();
		X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, QFAAudio3D::X3DInstance);
		IsInit = true;
	}
}

// https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--integrate-x3daudio-with-xaudio2
void QFAAudio3D::UpdateParameters()
{
	Emitter.ChannelCount = 2;
	Emitter.DopplerScaler = DopplerScaler;
	Emitter.CurveDistanceScaler = DistanceScaler;
	Emitter.InnerRadius = InnerRadius;
	Emitter.InnerRadiusAngle = InnerRadiusAngle;

	float azimuths[4] = { -1, 0 , 1, 0 };
	Emitter.pChannelAzimuths = azimuths; //  left microphone is at (−1,0), right microphone is at (+1,0) 
	Emitter.ChannelRadius = 1.0f;

	X3DAUDIO_DSP_SETTINGS DSPSettings = {};
	DSPSettings.SrcChannelCount = 2;
	DSPSettings.DstChannelCount = 2;
	DSPSettings.pMatrixCoefficients = Matrix;

	X3DAudioCalculate(QFAAudio3D::X3DInstance, &Listener, &Emitter,
		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER |
		X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB ,
		&DSPSettings);

	Audio.XAudio2SourceVoice->SetOutputMatrix(Audio.XAudio2MasteringVoice, 2, 2, DSPSettings.pMatrixCoefficients);
	Audio.XAudio2SourceVoice->SetFrequencyRatio(DSPSettings.DopplerFactor);

	XAUDIO2_FILTER_PARAMETERS FilterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * DSPSettings.LPFDirectCoefficient), 1.0f };
	Audio.XAudio2SourceVoice->SetFilterParameters(&FilterParameters);
}

void QFAAudio3D::SetPositionListener(FVector position, bool needUpdate)
{
	Listener.Position = X3DAUDIO_VECTOR{ position.X, position.Y, position.Z };
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetPositionEmitter(FVector position, bool needUpdate)
{
	Emitter.Position = X3DAUDIO_VECTOR{ position.X, position.Y, position.Z };
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetVelocityListener(FVector velocity, bool needUpdate)
{
	Listener.Velocity = { velocity.X, velocity.Y, velocity.Z };
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetVelocityEmitter(FVector velocity, bool needUpdate)
{
	Emitter.Velocity = { velocity.X, velocity.Y, velocity.Z };
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetFUVectorListener(FVector forvard, FVector up, bool needUpdate)
{
	Listener.OrientFront = { forvard.X, forvard.Y, forvard.Z };
	Listener.OrientTop = { up.X, up.Y, up.Z};

	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetDopplerScaler(float scale, bool needUpdate)
{
	DopplerScaler = scale;
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetDistanceScaler(float scale, bool needUpdate)
{
	DistanceScaler = scale;
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetInnerRadius(float radius, bool needUpdate)
{
	InnerRadius = radius;
	if (needUpdate)
		UpdateParameters();
}

void QFAAudio3D::SetInnerRadiusAngle(float Angle, bool needUpdate)
{
	InnerRadiusAngle = Angle;
	if (needUpdate)
		UpdateParameters();
}


void QFAAudio3D::SetAudio(const std::u32string& fileName, bool isAudioStream, const size_t bufferSize)
{
	Emitter.OrientFront = X3DAUDIO_VECTOR{ 1, 0, 0 };
	Emitter.OrientTop = X3DAUDIO_VECTOR{ 0, 0, 1 };
	Listener.OrientFront = X3DAUDIO_VECTOR{ 1, 0, 0 };
	Listener.OrientTop = X3DAUDIO_VECTOR{ 0, 0, 1 };
	Audio.SetAudio(fileName, isAudioStream, bufferSize);
	Init();
}
