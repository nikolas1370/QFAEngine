#include "FramebufferMain.h"
#include <Render/Shader/ShaderProgram.h>
#include <Render/Buffer/Framebuffer.h>
#include <Tools/Debug/OpenGlStuff.h>
#include <Render/Window/Viewport.h>

float QFAFrameBufferMain::VertexMain[] =
{
	-1.0f, -1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
};

unsigned int QFAFrameBufferMain::indexsMain[] =
{
	0,1,2,2,3,0
};
GLuint QFAFrameBufferMain::IBOMain ;
GLuint QFAFrameBufferMain::VBOMain;
unsigned int QFAFrameBufferMain::VAOMain;
QFAShaderProgram* QFAFrameBufferMain::ProgramMain ;

void QFAFrameBufferMain::Init(int width, int height)
{	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLCall(glGenVertexArrays(1, &VAOMain));
	GLCall(glBindVertexArray(VAOMain));

	GLCall(glGenBuffers(1, &VBOMain));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBOMain));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexMain), VertexMain, GL_STATIC_DRAW));

	glEnableVertexAttribArray(0);
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0));
	glEnableVertexAttribArray(1);
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)8));

	glGenBuffers(1, &IBOMain); // gen index buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOMain));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexsMain), indexsMain, GL_STATIC_DRAW));

	ProgramMain = new QFAShaderProgram("Engine/Shaders/MainFrameBuffer/Vertex.shader", "Engine/Shaders/MainFrameBuffer/fragment.shader");
}

void QFAFrameBufferMain::CopyFrameBuffer(QFAViewport* view)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, view->secondFrameBuffer.frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// glBlitFramebuffer — copy a block of pixels from the read framebuffer to the draw framebuffer
	glBlitFramebuffer(0, 0, view->Width, view->Height, view->X, view->Y, view->Width + view->X, view->Height + view->Y, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void QFAFrameBufferMain::BlankScreen(int w, int h)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	// glBlitFramebuffer — copy a block of pixels from the read framebuffer to the draw framebuffer
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}