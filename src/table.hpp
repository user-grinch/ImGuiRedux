#pragma once
#include <vector>

// Basic table implementation inspired by lua tables
template <typename S, typename T>
class Table 
{
private:
    std::vector<std::pair<S,T>> _data;
public:

    void Clear()
    {
        _data.clear();
    }

    T& operator[](S str)
    {
        for (auto it = _data.begin(); it != _data.end(); ++it)
        {
            // value exists, return ref
            if (it->first == str)
            {
                return it->second;
            }
        }
        
        // push the value into vector and return a ref
        _data.push_back({ str, T{}});
        return _data.back().second;
    }
};