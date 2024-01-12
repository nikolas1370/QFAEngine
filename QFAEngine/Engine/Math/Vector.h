#pragma once
#include "Math.h"
#include <iostream>


template<typename T> class TVector;

using FVector = TVector<float>;
using DVector = TVector<double>;


template<typename T>
class TVector
{	
public:
	T X; // Roll
	T Y; // Pitch
	T Z; // Yaw

    /*(1, 0, 0)*/
    static const TVector<T> ForwardVector;
    /*(-1, 0, 0)*/
    static const TVector<T> BackwardVector;
    /*(0, 1, 0)*/
    static const TVector<T> RightVector;
    /*(0, -1, 0)*/
    static const TVector<T> LeftVector;
    /*(0, 0, 1)*/
    static const TVector<T> UpVector;
    /*(0, 0, -1)*/
    static const TVector<T> DownVector;
    /*(0,0,0)*/
    static const TVector<T> ZeroVector;

    TVector<T> operator+(const TVector<T>& V) const
    {
        return TVector<T> { X + V.X, Y + V.Y, Z + V.Z };
    }

    TVector<T> operator+(T V) const
    {
        return TVector<T> { X + V, Y + V, Z + V };
    }

    TVector<T> operator-(const TVector<T>& V) const
    {
        return TVector<T> { X - V.X, Y - V.Y, Z - V.Z };
    }

    TVector<T> operator-(T V) const
    {
        return TVector<T> { X - V, Y - V, Z - V };
    }

    TVector<T> operator*(T V) const
    {
        return TVector<T> { X * V, Y * V, Z * V };
    }    

    TVector<T> operator/(T V) const
    {
        return TVector<T> { X / V, Y / V, Z / V };
    }

    

    TVector<T> operator*(const TVector<T>& V) const
    {
        return TVector<T>(X * V.X, Y * V.Y, Z * V.Z);
    }

    TVector<T> operator*(TVector& V) const
    {        
        return TVector<T>(X * V.X, Y * V.Y, Z * V.Z);
    }

    void operator=(const TVector<T>& V) 
    {
        X = V.X;
        Y = V.Y;
        Z = V.Z;
    }
    



    void operator+=(T V)
    {
        X += V;
        Y += V;
        Z += V;
    }

    void operator+=(const TVector<T>& V)
    {
        X += V.X;
        Y += V.Y;
        Z += V.Z;
    }

    void operator-=(T V)
    {
        X -= V;
        Y -= V;
        Z -= V;
    }

    void operator-=(const TVector<T>& V)
    {
        X -= V.X;
        Y -= V.Y;
        Z -= V.Z;
    }

    void operator*=(T V)
    {
        X *= V;
        Y *= V;
        Z *= V;
    }

    void operator*=(TVector& V)
    {
        X *= V.X;
        Y *= V.Y;
        Z *= V.Z;
    }


    T& operator[](int i) //T& because you can write in someVecto[0] = 1.f(instead someVecto.X = 1.f)
    {
        switch (i)
        {
        case 0: return X;
        case 1: return Y;
        case 2: return Z;
        default:__debugbreak(); return X;
        }
    }
    
    template<typename T>
    friend std::ostream& operator <<(std::ostream& os, const TVector<T>& c);

    
    TVector();
	TVector(T InF);
	TVector(T X, T Y, T Z);
    TVector(const glm::vec3 &lo);


    /*
    * Axis must be length 1
    */
	inline TVector<T> RotateAngleAxis(const T AngleDeg, const TVector<T>& Axis) const;
	TVector<T> RotateAngleAxisRad(const T AngleRad, const TVector<T>& Axis) const;

    /*
    * @return length vector
    */
    inline T Length() const;
    /**
     * @return normalized vector
     */
    inline TVector<T> Normalize() const;

    TVector<T> Cros(TVector<T> vec) const
    {
        return TVector<T>
        (
            Y * vec.Z - Z * vec.Y,
            Z * vec.X - X * vec.Z,
            X * vec.Y - Y * vec.X
        );
    }

    inline glm::vec3 GetGLMVector() const
    {
        return glm::vec3(X, Y, Z);
    }

    inline TVector<T> ConvertToVulkanCoordinate() const
    {
        return TVector<T>(Y, Z * -1, X);
    }

    inline TVector<T> ConvertFromVulkanCoordinate() const
    {
        return TVector<T>(Z, X, Y * -1);
    }

    inline bool equel(const TVector<T>& v, T around = 0.00001) const
    {
        return (v.X - X < around && v.Y - Y < around && v.Z - Z < around);
    }
    inline bool equel(const TVector<T>* v, T around = 0.00001) const
    {
        return (v->X - X < around && v->Y - Y < around && v->Z - Z < around);
    }

    inline TVector<T> Floor() const
    {
        return TVector<T>(floor(X), floor(Y), floor(Z));
    }

    /*
    *  calculate a reverse scale factor
    *
    * 
    * 
    */
    TVector<T> ReversScale(const TVector<T>& scale) const
    {
        TVector<T> _floor = scale.Floor(); // (scale.X < 1 && scale.X)  protect from divide by zero
        return TVector<T>(X * (scale.X < 1 && scale.X > -1 ? scale.X : (_floor.X / scale.X) / _floor.X),
                          Y * (scale.Y < 1 && scale.Y > -1 ? scale.Y : (_floor.Y / scale.Y) / _floor.Y),
                          Z * (scale.Z < 1 && scale.Z > -1 ? scale.Z : (_floor.Z / scale.Z) / _floor.Z));
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const TVector<T>& c)
{
    return os << "X=" << c.X << ", Y=" << c.Y << ", Z=" << c.Z;
}



template<typename T> const TVector<T> TVector<T>::ForwardVector(1, 0, 0);
//template<typename T> const TVector<T> TVector<T>::BackwardVector(-1, 0, 0);
template<typename T> const TVector<T> TVector<T>::RightVector(0, 1, 0);
//template<typename T> const TVector<T> TVector<T>::LeftVector(0, -1, 0);
template<typename T> const TVector<T> TVector<T>::UpVector(0, 0, 1);
//template<typename T> const TVector<T> TVector<T>::DownVector(0, 0, -1);
//template<typename T> const TVector<T> TVector<T>::ZeroVector(0, 0, 0);


template<typename T>
TVector<T>::TVector() 
    : X(0), Y(0), Z(0)
{
}

template<typename T>
TVector<T>::TVector(T InF)
	: X(InF), Y(InF), Z(InF)
{

}

template<typename T>
TVector<T>::TVector(T InX, T InY, T InZ)
	: X(InX), Y(InY), Z(InZ)
{
}

template<typename T>
inline TVector<T>::TVector(const glm::vec3& lo)
    : X(lo.x), Y(lo.y), Z(lo.z)
{

}



template<typename T>
inline TVector<T> TVector<T>::RotateAngleAxis(const T AngleDeg, const TVector<T>& Axis) const
{
	return RotateAngleAxisRad(Math::DegreesToRadians(AngleDeg), Axis);
}

template<typename T>
TVector<T> TVector<T>::RotateAngleAxisRad(const T AngleRad, const TVector<T>& Axis) const
{
    float S = sin(AngleRad), C = cos(AngleRad);
    //FMath::SinCos(&S, &C, AngleRad);

    const float XX = Axis.X * Axis.X;
    const float YY = Axis.Y * Axis.Y;
    const float ZZ = Axis.Z * Axis.Z;

    const float XY = Axis.X * Axis.Y;
    const float YZ = Axis.Y * Axis.Z;
    const float ZX = Axis.Z * Axis.X;

    const float XS = Axis.X * S;
    const float YS = Axis.Y * S;
    const float ZS = Axis.Z * S;

    const float OMC = 1.f - C;
    // 3x3 rotate matrix by 3x1 vector // 3x3 Rotation about in arbitrary axis
    return TVector<T>{// M[0][0] = (OMC * XX + C), M[0][1] = (OMC * XY - ZS), ...,  M[2][2] = (OMC * ZZ + C)
        (OMC * XX + C) * X + (OMC * XY - ZS) * Y + (OMC * ZX + YS) * Z,
        (OMC * XY + ZS) * X + (OMC * YY + C) * Y + (OMC * YZ - XS) * Z,
        (OMC * ZX - YS) * X + (OMC * YZ + XS) * Y + (OMC * ZZ + C) * Z
    };
}
template<typename T>
inline T TVector<T>::Length() const
{
    return sqrt(X * X + Y*Y + Z*Z);
}

/*
do 
FVector NormalizeOut() const;
void Normalize();
 */
template<typename T>
inline TVector<T> TVector<T>::Normalize() const
{    
    T len = Length();
    if (len < 0.00001 && len > -0.00001)
        return TVector<T>(0);

    return TVector<T>{X / len, Y / len, Z / len};
}

