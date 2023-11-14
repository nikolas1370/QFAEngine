#pragma once
#include <Tools/Debug/OpenGlStuff.h>

class QFAWindow;
class QFAViewport;
class QFAFrameBuffer;
class QFAShaderProgram;

class QFAFrameBufferMain
{
	friend QFAWindow;
	static unsigned int VAOMain;
	static float VertexMain[];
	static unsigned int indexsMain[];
	static GLuint IBOMain;
	static GLuint VBOMain;
	static QFAShaderProgram* ProgramMain;

	static void Init(int width, int height);
	static void CopyFrameBuffer(QFAViewport* view);
	static void BlankScreen(int w, int h);
public:
	

private:

};

