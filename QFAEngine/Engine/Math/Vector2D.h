#pragma once
#include "Math.h"
#include "iostream"

template<typename T> class TVector2D;
using DVector2D = TVector2D<double>;
using FVector2D = TVector2D<float>;

/*
rewrite like TVector


*/
template<typename T>
class TVector2D
{
public:
    T X;
    T Y;

    TVector2D(){}
    TVector2D(T InF);
    TVector2D(T X, T Y);

    TVector2D<T> operator+(TVector2D<T> V)
    {
        return TVector2D<T> { X + V.X, Y + V.Y };
    }

    TVector2D<T> operator+(T V)
    {
        return TVector2D<T> { X + V, Y + V };
    }

    TVector2D<T> operator-(TVector2D<T> V)
    {
        return TVector2D<T> { X - V.X, Y - V.Y };
    }

    TVector2D<T> operator-(T V)
    {
        return TVector2D<T> { X - V, Y - V };
    }

    TVector2D<T> operator*(T V)
    {
        return TVector2D<T> { X * V, Y * V };
    }

    void operator=(TVector2D<T> V)
    {
        X = V.X;
        Y = V.Y;
    }

    void operator+=(T V)
    {
        X += V;
        Y += V;
    }

    void operator+=(TVector2D<T> V)
    {
        X += V.X;
        Y += V.Y;
    }

    void operator-=(T V)
    {
        X -= V;
        Y -= V;
    }

    void operator-=(TVector2D<T> V)
    {
        X -= V.X;
        Y -= V.Y;
    }

    void operator*=(T V)
    {
        X *= V;
        Y *= V;
    }

    void operator/=(T V)
    {
        X /= V;
        Y /= V;
    }

    T& operator[](int i)
    {
        switch (i)
        {
            case 0: return X;
            case 1: return Y;
            default:__debugbreak(); return X;
        }
    }

    template<typename T>
    friend std::ostream& operator <<(std::ostream& os, const TVector2D<T>& c)
    {
        os << "X=" << c.X << ", Y=" << c.Y;
        return os;
    }

    /*
    * Axis must be length 1
    */
    inline TVector2D<T> RotateAngle(const T AngleDeg);
    TVector2D<T> RotateAngleRad(const T AngleRad);

    /*
    * @return length vector
    */
    inline T Length() const;
    /**
     * @return normalized vector
     */
    inline TVector2D<T> Normalize();


};


template<typename T>
inline TVector2D<T>::TVector2D(T InF)
    : X(InF), Y(InF)
{
}

template<typename T>
inline TVector2D<T>::TVector2D(T InX, T InY)
    : X(InX), Y(InY)
{
}

template<typename T>
inline TVector2D<T> TVector2D<T>::RotateAngle(const T AngleDeg)
{
    return RotateAngleRad(Math::DegreesToRadians(AngleDeg));
}

template<typename T>
inline TVector2D<T> TVector2D<T>::RotateAngleRad(const T AngleRad)
{
    T S = sin(AngleRad), C = cos(AngleRad);
    return TVector2D<T>
    {
        X * C - Y * S,
        X * S + Y * C
    };
}

template<typename T>
inline T TVector2D<T>::Length() const
{    
    return sqrt(X*X + Y*Y);
}

template<typename T>
inline TVector2D<T> TVector2D<T>::Normalize()
{
    T len = Length();
    return TVector2D<T>(X / len, Y / len);
}
