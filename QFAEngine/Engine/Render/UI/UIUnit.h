#pragma once

class QFAUIUnit
{
protected:
	unsigned int Width = 300;
	unsigned int Height = 120;
	int Position_x = 0;
	// 0 == top 
	int Position_y = 0;

	bool CanResize = true;
public:
	virtual void SetPosition(unsigned int x, unsigned int y) = 0;
	virtual void SetSize(unsigned int w, unsigned int h) = 0;


private:

};

