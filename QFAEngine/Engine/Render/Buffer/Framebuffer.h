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
	void StartFrame();


	QFAFrameBuffer();
	~QFAFrameBuffer();

private:

};