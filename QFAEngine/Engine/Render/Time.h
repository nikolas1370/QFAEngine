#pragma once
#include <GLFW/glfw3.h>
class Render;


class QTime // microseconds
{
public:
	// call after glfwInit
	static void Init(); // remove to private cal from render or else

private:
	friend Render;
	static uint64_t StartTime;
	static uint64_t WorkTime; // not count when ps sleep

	static double DeltaTime;
	static uint64_t LastTime;
	
	
	
	
public:

	/*
		call every frame
		call before work with time
	*/
	// in private
	static void CalcDeltaTime();


	static inline double GetDeltaTime()
	{
		return DeltaTime;
	}

	// in 1 / 10 microseconds(system up time) (0.000 000 1)  from start program
	static inline uint64_t GetTime()
	{// in future
		return glfwGetTimerValue() - StartTime;
	}

	/*
		in 1 / 10 microseconds(system up time) (0.000 000 1) 
		count only when program work( pc(program) not sleep )
	*/
	static inline uint64_t GetWorkTime()
	{
		return (glfwGetTimerValue() - WorkTime) + WorkTime;
	}

	// in 1/10 microseconds (system up time) (0.000 000 1)
	static inline uint64_t GetSystemTime()
	{
		return glfwGetTimerValue();
	}
	/*
	static inline double GetTimeDoublePrecision()
	{
		return glfwGetTime();
	}
	*/
private:

};



