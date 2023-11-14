#pragma once

class QFAViewport;
class QFAFrameBufferMain;
class QFAFrameBufferMain;
class QFAFrameBuffer
{
	friend QFAViewport;
	friend QFAFrameBufferMain;
	friend QFAFrameBufferMain;
	unsigned int frameBuffer;
	unsigned int colorBuffer;
	unsigned int rbo;

	void UpdateSize(int w, int h);
	void StartFrame();
public:
	


	QFAFrameBuffer();
	QFAFrameBuffer(int w, int h);
	
	~QFAFrameBuffer();

private:
	void Init(int w, int h);

};