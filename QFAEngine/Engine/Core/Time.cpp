#include "pch.h"
#include "Time.h"
#include <Core/EngineStuff/VulkanSuff.h>
#include <Tools/Stuff.h>
#include <GLFW/glfw3.h>
uint64_t QTime::StartTime = 0;
uint64_t QTime::WorkTime = 0;
double QTime::DeltaTime = 0.0;
uint64_t QTime::LastTime = 0;

void  QTime::Init()
{
	if(StartTime == 0)
	{
		if (!glfwInit())
			stopExecute("!glfwInit()")

		StartTime = glfwGetTimerValue();
	}
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

double QTime::GetDeltaTime()
{
	return DeltaTime;
}

uint64_t QTime::GetTime()
{// in future
	return glfwGetTimerValue() - StartTime;
}

uint64_t QTime::GetWorkTime()
{
	return (glfwGetTimerValue() - WorkTime) + WorkTime;
}

uint64_t QTime::GetSystemTime()
{
	return glfwGetTimerValue();
}
