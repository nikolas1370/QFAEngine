#include "Framebuffer.h"


FrameBuffer::FrameBuffer()
{
	GLCall(glGenFramebuffers(1, &framebuffer));
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &framebuffer));
}
