#include "Framebuffer.h"
#include <Tools/Debug/OpenGlStuff.h>
#include <iostream>

QFAFrameBuffer::QFAFrameBuffer()
{
	Init(600, 600);

}

QFAFrameBuffer::QFAFrameBuffer(int w, int h)
{
	Init( w, h);
}

QFAFrameBuffer::~QFAFrameBuffer()
{

}

void QFAFrameBuffer::Init(int w, int h)
{
	Width = w;
	Height = h;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);//GL_FRAMEBUFFER read and write (GL_DRAW_FRAMEBUFFER only write)

	glClearColor(0.1f, 0.1f, 0.1f, 1);
	/*---------  i Gen Renderbuffers for color*/
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &rbo);// attach DEPTH STENCIL buffer
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	/*---  attach color_renderbuffer*/
	// in EndFrame copy from second frame bufer to  defauld frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "!--------------------------------" << std::endl;
}

void QFAFrameBuffer::StartFrame()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, Width, Height);
	

	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));// settup how opengl by with alpha
	GLCall(glEnable(GL_BLEND));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void QFAFrameBuffer::UpdateSize(int w, int h)
{
	Width = w;
	Height = h;
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
}
