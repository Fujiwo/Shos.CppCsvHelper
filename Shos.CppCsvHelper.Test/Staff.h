#pragma once

#include <string>
using namespace std;

struct Staff
{
    enum class Kind
    {
        Kind1, Kind2, Kind3
    };

    Staff(unsigned long number = 0L, wstring name = L"", Kind kind = Kind::Kind1, double height = 0.0, float weight = 0.0f)
        : number(number), name(name), kind(kind), height(height), weight(weight)
    {}

    unsigned long number;
    wstring name;
    Kind kind;
    double height;
    float weight;
};

inline wstring to_wstring(Staff::Kind value)
{
    return to_wstring((int)value);
}
