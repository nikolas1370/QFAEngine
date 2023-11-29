#pragma once

#include <vector>
/* */
template <typename T>
class QFAArray
{
    friend QFAArray;
    std::vector<T> vector;
public:
    QFAArray() 
    {
        
    }

    QFAArray(const QFAArray& copy) 
    {
        vector = copy.vector;
    }
    
    
    void operator=(const QFAArray<T>& copy)
    {
        vector = copy.vector;
    }

    ~QFAArray()
    {        

    }

    inline void Add(const T& some)
    {
        vector.push_back(some);
    }

    bool Remove(const T& t)
    {
        for (int i = 0; i < vector.size(); i++)
        {
            if (vector[i] == t)
            {                
                vector[i] = vector[vector.size() - 1];
                vector.pop_back();
                return true;
            }
        }

        return false;
    }

    void RemoveAt(size_t index)
    {
        if (index < 0 || index >= vector.size())
            return;

        vector[index] = vector[vector.size() - 1];
        vector.pop_back();
    }
    

    T& operator[](size_t i)
    {
        return vector[i];
    }

    
    T& Last()
    {
        return vector.back();
    }

    inline size_t Length()
    {
        return vector.size();
    }

    
private:

};

