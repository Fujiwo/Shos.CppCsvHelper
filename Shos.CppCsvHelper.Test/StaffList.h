#pragma once
#include <vector>
#include <fstream>
#include <codecvt>
#include <functional>
#include <tuple>
using namespace std;
#include <cassert>
#include "Staff.h"
#include "../Shos.CppCsvHelper/CsvHelper.h"

class StaffList
{
    vector<Staff> staffs;

public:
    using iterator = vector<Staff>::const_iterator;

    const wstring fileName = L"stafflist.csv";

    StaffList()
    {}

    StaffList(initializer_list<Staff> init)
        : staffs(init.begin(), init.end())
    {}

    void push_back(const Staff& staff)
    {
        staffs.push_back(staff);
    }

    iterator begin() const
    {
        return staffs.cbegin();
    }

    iterator end() const
    {
        return staffs.cend();
    }

    void Save()
    {
        wofstream stream(fileName);
        if (!stream)
            return;
        stream.imbue(locale("", locale::ctype));
        //locale::global(locale(""));

        function<tuple<int, wstring, Staff::Kind, double, float>(const Staff&)> toTuple =
            [](const Staff& staff) { return make_tuple(staff.number, staff.name, staff.kind, staff.height, staff.weight); };

        Shos::CsvHelper::Write(stream, begin(), end(), toTuple);
        //csvHelper.Write(stream, begin(), end(), [](const Staff& staff) { return make_tuple(staff.number, staff.name, staff.kind); });
    }

    void Load()
    {
        wifstream stream(fileName);
        if (!stream)
            return;
        stream.imbue(locale("", locale::ctype));
        //locale::global(locale(""));

        function<tuple<int, wstring, Staff::Kind, double, float>(const vector<wstring>&)> toTuple =
            [](const vector<wstring>& csvTexts) {
            assert(csvTexts.size() == 5);
            auto number = static_cast<unsigned int>(stoul(csvTexts[0]));
            auto name   = csvTexts[1];
            auto kind   = static_cast<Staff::Kind>(stoi(csvTexts[2]));
            auto height = stod(csvTexts[3]);
            auto weight = stof(csvTexts[4]);
            return make_tuple(number, name, kind, height, weight);
        };

        auto elements = Shos::CsvHelper::Read<Staff>(stream, toTuple);
        staffs = elements;
    }
};

