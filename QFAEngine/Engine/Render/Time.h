#pragma once
#include <Tools/Stuff.h>
#include <stdint.h>

class Render;
class QFAEXPORT QTime // microseconds
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


	static double GetDeltaTime();
	

	// in 1 / 10 microseconds(system up time) (0.000 000 1)  from start program
	static  uint64_t GetTime();

	/*
		in 1 / 10 microseconds(system up time) (0.000 000 1) 
		count only when program work( pc(program) not sleep )
	*/
	static uint64_t GetWorkTime();


	// in 1/10 microseconds (system up time) (0.000 000 1)
	static uint64_t GetSystemTime();
private:

};



