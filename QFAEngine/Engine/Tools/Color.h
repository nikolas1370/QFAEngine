#pragma once

class QFAColorF
{
public:
	float R;
	float G;
	float B;
	float A;

	QFAColorF(){}

	QFAColorF(float r, float g, float b)
	{
		R = r;
		G = g;
		B = b;
		A = 1;
	}

	QFAColorF(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
private:

};

class QFAColorB
{
public:
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
	QFAColorB() {}
	QFAColorB(unsigned char r, unsigned char g, unsigned char b)
	{
		R = r;
		G = g;
		B = b;
		A = 1;
	}

	QFAColorB(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}



private:

};


class QFAColor
{
public:
	int R;
	int G;
	int B;
	int A;

	QFAColor() {}

	QFAColor(int all)
	{
		R = all;
		G = all;
		B = all;
		A = 1;
	}

	QFAColor(int r, int g, int b) 
	{
		R= r;
		G= g;
		B= b;
		A= 1;
	}

	QFAColor(int r, int g, int b, int a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}

	QFAColor(QFAColorB color)
	{
		R = color.R;
		G = color.G;
		B = color.B;
		A = color.A;
	}

	QFAColor(QFAColorF color)
	{
		R = (int)(color.R * 255.0f);
		G = (int)(color.G * 255.0f);
		B = (int)(color.B * 255.0f);
		A = (int)(color.A * 255.0f);
	}


	QFAColorB GetColorB()
	{		
		return QFAColorB
		{
			(unsigned char)(R < 0 ? 0 : (R > 255 ? 255 : R)),
			(unsigned char)(G < 0 ? 0 : (G > 255 ? 255 : G)),
			(unsigned char)(B < 0 ? 0 : (B > 255 ? 255 : B)),
			(unsigned char)(A < 0 ? 0 : (A > 255 ? 255 : A)),
		};
	}

	QFAColorF GetColorF()
	{
		return QFAColorF
		{
			(float)(R < 0 ? 0.0f : (R > 255 ? 1.0f : (float)R / 255.0f)),
			(float)(G < 0 ? 0.0f : (G > 255 ? 1.0f : (float)G / 255.0f)),
			(float)(B < 0 ? 0.0f : (B > 255 ? 1.0f : (float)B / 255.0f)),
			(float)(A < 0 ? 0.0f : (A > 255 ? 1.0f : (float)A / 255.0f))
		};
	}

private:

};
