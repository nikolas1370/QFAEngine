#pragma once

#include <vector>
/* */
template <typename T>
class QFAArray
{
    friend QFAArray;
    std::vector<T> vector;
    size_t size = 0;
public:
    QFAArray() 
    {
        
    }

    QFAArray(const QFAArray& copy) 
    {
        vector = copy.vector;
        size = copy.size;
    }
    
    
    void operator=(const QFAArray<T>& copy)
    {
        vector = copy.vector;
        size = copy.size;
    }

    ~QFAArray()
    {        

    }

    inline void Add(const T& some)
    {
        if (size == vector.size())
            vector.push_back(some);
        else
            vector[size] = some;

        size++;
    }

    bool Remove(const T& t)
    {
        for (int i = 0; i < size; i++)
        {
            if (vector[i] == t)
            {                
                vector[i] = vector[--size];
                return true;
            }
        }

        return false;
    }

    void RemoveAt(size_t index)
    {
        if (index >= vector.size())
            return;

        vector[index] = vector[--size];        
    }
    

    T& operator[](size_t i)
    {        
        return vector[i];
    }

    
    T& Last()
    {
        return vector[size - 1];
    }

    inline size_t Length()
    {
        return size;
    }

    inline void Clear()
    {
        size = 0;
    }    

    inline T* Data()
    {
        return vector.data();
    }

    inline void Reserve(size_t size)
    {
        vector.reserve(size);
    }
};

