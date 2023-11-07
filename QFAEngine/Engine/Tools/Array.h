#pragma once
/* create for pointer and enum */
template <typename T>
class QFAArray
{
    int MaxAmount = 0;
    int Amount = 0;
    T* Data;

public:
    QFAArray() {}

    ~QFAArray()
    {
        delete[] Data;
    }

    void Add(T t)
    {
        if (MaxAmount == 0)
        {
            MaxAmount = 10;
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

        Data[Amount++] = t;
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

    void RemoveAt(int index)
    {
        if (index < 0 || index >= Amount)
            return;

        Data[index] = Data[--Amount];
    }
    

    T& operator[](int i)
    {
        return Data[i];
    }

    inline int Length()
    {
        return Amount;
    }
private:

};

