#pragma once
#include <Tools/Debug/OpenGlStuff.h>

/*
Difference between render buffer attachment and texture attachment

The difference between them is that a texture allow upload and read pixels from c++ program (CPU). 
The render buffers are always in the GPU and super fast. 
*/

class FrameBuffer
{
	unsigned int framebuffer;

public:
	FrameBuffer();
	~FrameBuffer();

private:

};

