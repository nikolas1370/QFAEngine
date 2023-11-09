#pragma once

class QFARender;
class QFAFrameBufferMain;
class QFAFrameBuffer
{
	friend QFARender;
	friend QFAFrameBufferMain;
	unsigned int frameBuffer;
	unsigned int colorBuffer;
	unsigned int rbo;

public:
	void StartFrame(bool newParameter = false, int w = 0, int h = 0);


	QFAFrameBuffer();
	QFAFrameBuffer(int w, int h);
	
	~QFAFrameBuffer();

private:
	void Init(int w, int h);

};