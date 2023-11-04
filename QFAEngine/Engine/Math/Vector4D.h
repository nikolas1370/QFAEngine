#pragma once
#include <iostream>
#include "Vector.h"
#include "Matrix.h"

template<typename T> class TVector4D;
using DVector4D = TVector4D<double>;
using FVector4D = TVector4D<float>;

struct Matrix;

template<typename T>
class TVector4D
{// for matrix
public:
	T X;
	T Y;
	T Z;
	T W;
	TVector4D();	
	TVector4D(TVector<T> V, T v4);
	TVector4D(T InF);
	TVector4D(T InX, T InY, T InZ, T InW);
	
	T& operator[](int i)
	{
		switch (i)
		{			
			case 0: return X;
			case 1: return Y;
			case 2: return Z;
			case 3: return W;
			default:__debugbreak(); return X;
		}
	}

	void operator+=(T V)
	{
		 X += V, Y += V, Z += V, W += V;
	}

	
	TVector4D<T> operator+(T V)
	{
		return TVector4D<T> { X + V, Y + V, Z + V , W + V};
	}

	float operator*(TVector4D<T> V)// dot product
	{		
		return  X * V.X + Y * V.Y + Z * V.Z + W * V.W;
	}


	TVector4D<T> operator*( Matrix& M)
	{
		TVector4D<T> nv(0); 		
		nv[0] = X * M[0][0] + Y * M[1][0] + Z * M[2][0] + W * M[3][0];
		nv[1] = X * M[0][1] + Y * M[1][1] + Z * M[2][1] + W * M[3][1];
		nv[2] = X * M[0][2] + Y * M[1][2] + Z * M[2][2] + W * M[3][2];
		nv[3] = X * M[0][3] + Y * M[1][3] + Z * M[2][3] + W * M[3][3];
		return nv;
	}

	friend std::ostream& operator<<(std::ostream& os, const TVector4D<T>&& c)
	{
		return os << "X=" << c.X << ", Y=" << c.Y << ", Z=" << c.Z << ", W=" << c.W;
		//return os;
	}

};

template<typename T>
inline TVector4D<T>::TVector4D()
{
}

template<typename T>
inline TVector4D<T>::TVector4D(TVector<T> V, T v4)
	: X(V.X), Y(V.Y), Z(V.Z), W(v4)
{

}



template<typename T>
TVector4D<T>::TVector4D(T InF)
	: X(InF), Y(InF), Z(InF), W(InF)
{
}

template<typename T>
inline TVector4D<T>::TVector4D(T InX, T InY, T InZ, T InW)
	: X(InX), Y(InY), Z(InZ), W(InW)
{
}
/*
template<typename T>
inline TVector4D<T> TVector4D<T>::operator*(Matrix M)
{
	return TVector4D<T>();
}
*/
