#pragma once
#include <iostream>
#include "Vector4D.h"

struct Matrix
{// колись думав зробить свої матриці з копіювавши glm но стало впадло і удалив всі нароботки
    FVector4D Mat[4];
    Matrix(){}
    
    Matrix(float v)
    {
        float* it = (float*)this;
        for (int i = 0; i < sizeof(Matrix) / sizeof(float); i++)
        {
            *it = v;
            it++;
        }
    }


    

    Matrix(FVector4D r1, FVector4D r2, FVector4D r3, FVector4D r4)
    {
        Mat[0] = r1;
        Mat[1] = r2;
        Mat[2] = r3;
        Mat[3] = r4;
    }
    

    Matrix(float r1c1, float r1c2, float r1c3, float r1c4,
        float r2c1, float r2c2, float r2c3, float r2c4,
        float r3c1, float r3c2, float r3c3, float r3c4,
        float r4c1, float r4c2, float r4c3, float r4c4)
    {
        Mat[0][0] = r1c1;
        Mat[0][1] = r1c2;
        Mat[0][2] = r1c3;
        Mat[0][3] = r1c4;

        Mat[1][0] = r2c1;
        Mat[1][1] = r2c2;
        Mat[1][2] = r2c3;
        Mat[1][3] = r2c4;

        Mat[2][0] = r3c1;
        Mat[2][1] = r3c2;
        Mat[2][2] = r3c3;
        Mat[2][3] = r3c4;

        Mat[3][0] = r4c1;
        Mat[3][1] = r4c2;
        Mat[3][2] = r4c3;
        Mat[3][3] = r4c4;
    }



    FVector4D& operator[](int i)
    {// for [][]
        return Mat[i];
    }

 

    Matrix operator*(Matrix& M)
    {
        Matrix nm;
        Matrix& it = *this;

        nm[0][0] = it[0] * FVector4D{ M[0][0], M[1][0], M[2][0], M[3][0] };
        nm[0][1] = it[0] * FVector4D{ M[0][1], M[1][1], M[2][1], M[3][1] };
        nm[0][2] = it[0] * FVector4D{ M[0][2], M[1][2], M[2][2], M[3][2] };
        nm[0][3] = it[0] * FVector4D{ M[0][3], M[1][3], M[2][3], M[3][3] };

        nm[1][0] = it[1] * FVector4D{ M[0][0], M[1][0], M[2][0], M[3][0] };
        nm[1][1] = it[1] * FVector4D{ M[0][1], M[1][1], M[2][1], M[3][1] };
        nm[1][2] = it[1] * FVector4D{ M[0][2], M[1][2], M[2][2], M[3][2] };
        nm[1][3] = it[1] * FVector4D{ M[0][3], M[1][3], M[2][3], M[3][3] };

        nm[2][0] = it[2] * FVector4D{ M[0][0], M[1][0], M[2][0], M[3][0] };
        nm[2][1] = it[2] * FVector4D{ M[0][1], M[1][1], M[2][1], M[3][1] };
        nm[2][2] = it[2] * FVector4D{ M[0][2], M[1][2], M[2][2], M[3][2] };
        nm[2][3] = it[2] * FVector4D{ M[0][3], M[1][3], M[2][3], M[3][3] };

        nm[3][0] = it[3] * FVector4D{ M[0][0], M[1][0], M[2][0], M[3][0] };
        nm[3][1] = it[3] * FVector4D{ M[0][1], M[1][1], M[2][1], M[3][1] };
        nm[3][2] = it[3] * FVector4D{ M[0][2], M[1][2], M[2][2], M[3][2] };
        nm[3][3] = it[3] * FVector4D{ M[0][3], M[1][3], M[2][3], M[3][3] };

        return nm;
    }


    FVector4D operator*(FVector4D& V)
    {        
        FVector4D nv;
        Matrix& M = *this;        
        nv[0] = M[0] * V;
        nv[1] = M[1] * V;
        nv[2] = M[2] * V;
        nv[3] = M[3] * V;
        return nv;
    }

    
    friend std::ostream& operator <<(std::ostream& os, Matrix& m)
    {
        return os << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ", " << std::endl
            << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ", " << std::endl
            << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ", " << std::endl
            << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ", " << std::endl;
    }



};