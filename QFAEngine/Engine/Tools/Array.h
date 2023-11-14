#pragma once
/* create for pointer and enum */
#include <iostream>

template <typename T>
class QFAArray
{
    size_t MaxAmount = 10;
    size_t Amount = 0;

    /*
        if call delete[] not allocate memory be error.
        if create QFAArray<SAxis1D>
            and delete after be error.
        SAxis1D hawe inside QFAArray<SKeyAxis1D> and after call
            ~QFAArray<SKeyAxis1D> delete[] give errror.
    */
    bool Alloc = false;          
    T* Data;

public:
    QFAArray() {}

    ~QFAArray()
    {
        if(Alloc)
            delete[] Data;
    }

    void Add(T some)
    {
        if (!Alloc)
        {
            Alloc = true;
            Data = new T[MaxAmount];
        }
        else if(Amount >= MaxAmount)
        {
            MaxAmount *= 2;
            T* tem = Data;
            Data = new T[MaxAmount];
            for (int i = 0; i < Amount; i++)
                Data[i] = tem[i];

            delete[] tem;            
        }

        Data[Amount++] = some;
    }

    bool Remove(T t)
    {
        for (int i = 0; i < Amount; i++)
        {
            if (Data[i] == t)
            {
                Data[i] = Data[--Amount];
                return true;
            }
        }

        return false;
    }

    void RemoveAt(size_t index)
    {
        if (index < 0 || index >= Amount)
            return;

        Data[index] = Data[--Amount];
    }
    

    T& operator[](size_t i)
    {
        return Data[i];
    }

    inline size_t Length()
    {
        return Amount;
    }
private:

};

