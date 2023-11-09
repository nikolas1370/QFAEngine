#pragma once
#include <Tools/Debug/OpenGlStuff.h>

class QFARender;
class QFAFrameBuffer;
class QFAShaderProgram;
class QFAFrameBufferMain
{
	friend QFARender;
	static unsigned int VAOMain;
	static float VertexMain[];
	static unsigned int indexsMain[];
	static GLuint IBOMain;
	static GLuint VBOMain;
	static QFAShaderProgram* ProgramMain;

	static void Init();
	static void CopyFrameBuffer(QFAFrameBuffer* frameBuffer);
	static void BlankScreen();
public:
	

private:

};

