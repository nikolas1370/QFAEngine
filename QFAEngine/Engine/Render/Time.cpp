#include "Time.h"
uint64_t QTime::StartTime = 0;
uint64_t QTime::WorkTime = 0;
double QTime::DeltaTime = 0.0;
uint64_t QTime::LastTime = 0;

void  QTime::Init()
{
	StartTime = glfwGetTimerValue();
}

void QTime::CalcDeltaTime()
{
	uint64_t timu = glfwGetTimerValue() - StartTime;
	DeltaTime = (double)(timu - LastTime) * 0.0000001;// convert from 1/10 microseconds to second
	LastTime = timu;
	if (DeltaTime > 1.0)// if pc sleep and after some tame woke up and DeltaTimenot be super big
	{
		DeltaTime = 1.0;
		WorkTime += 1;
	}
	else
		WorkTime += timu;
}
