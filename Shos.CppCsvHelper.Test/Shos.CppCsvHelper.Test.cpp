#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
using namespace std;
#include "StaffList.h"
#include "stopwatch.h"

class ReadWritePerformanceTest
{
    inline static const wstring fileName = L"test.csv";

    struct Foo
    {
        int           m1;
        wstring       m2;
        double        m3;
        float         m4;
        unsigned long m5;
        char          m6;

        Foo(int m1, wstring m2, double m3, float m4, unsigned long m5, char m6)
            : m1(m1), m2(m2), m3(m3), m4(m4), m5(m5), m6(m6)
        {}

        wstring to_wstring() const
        {
            wostringstream stream;
            stream << L"m1(" << m1 << L"), "
                   << L"m2(" << m2 << L"), "
                   << L"m3(" << m3 << L"), "
                   << L"m4(" << m4 << L"), "
                   << L"m5(" << m5 << L"), "
                   << L"m6(" << m6 << L")";
            return stream.str();
        }
    };

public:
    void Save()
    {
        auto foos = GetRandomFoos(10000);

        wofstream stream(fileName);
        if (!stream)
            return;
        stream.imbue(locale("", locale::ctype));

        function<tuple<int, wstring, double, float, unsigned long, char>(const Foo&)> toTuple =
            [](const Foo& staff) { return make_tuple(staff.m1, staff.m2, staff.m3, staff.m4, staff.m5, staff.m6); };

        Shos::CsvHelper::Write(stream, foos.begin(), foos.end(), toTuple);
    }

    void Load()
    {
        wifstream stream(fileName);
        if (!stream)
            return;
        stream.imbue(locale("", locale::ctype));

        vector<Foo> elements;
        {
            stopwatch watch;
            function<Foo(const vector<wstring>&)> toTuple =
                [](const vector<wstring>& csvTexts) {
                assert(csvTexts.size() == 5);
                auto m1 = stoi(csvTexts[0]);
                auto m2 = csvTexts[1];
                auto m3 = stod(csvTexts[2]);
                auto m4 = stof(csvTexts[3]);
                auto m5 = stoul(csvTexts[4]);
                auto m6 = static_cast<char>(stoi(csvTexts[5]));
                return Foo(m1, m2, m3, m4, m5, m6);
            };

            elements = Shos::CsvHelper::Read<Foo>(stream, toTuple);
        }

        for (auto element : elements)
            wcout << element.to_wstring() << endl;

        {
            //const size_t bufferSize = 1;
            //wchar_t buffer[bufferSize];

            //stopwatch watch;
            //while (!stream.eof()) {
            //    stream.read(buffer, bufferSize);
            //}

            //wstring csv;
            //wstring textLine;
            //while (getline(stream, textLine))
            //    csv += textLine + L'\n';
        }
    }

private:
    static random_device random;
    static mt19937 mt;

    static vector<Foo> GetRandomFoos(size_t number)
    {
        vector<Foo> foos;
        for (size_t count = 0; count < number; count++)
            foos.push_back(GetRandomFoo());
        return foos;
    }

    static Foo GetRandomFoo()
    {
        return Foo(RandomValue<int>(-10000, 10000), GetRandomString(), RandomValue<double>(-100.0, 100.0), RandomValue<float>(-100.0f, 100.0f), RandomValue<unsigned long>(0UL, 1000000UL), RandomValue<char>('A', 'z'));
    }

    static wstring GetRandomString()
    {
        wstring text;
        auto stringLength = RandomValue<size_t>(3U, 80U);
        for (size_t count = 0; count < stringLength; count++) {
            auto charactor = GetRandomCharacter();
            charactor == 0x7f ? (text += L"\n") : (text += charactor);
        }
        return text;
    }

    static wchar_t GetRandomCharacter()
    {
        return RandomValue<wchar_t>(L' ', 0x7fL);
    }

    //static long RandomValue(long minimum, long maxmum)
    //{
    //    return minimum + mt() % (maxmum - minimum + 1);
    //};

    //static double RandomValue(double minimum, double maxmum)
    //{
    //    return GetRandomValue<double>(minimum, maxmum);
    //};

    template<typename Numeric, typename Generator = std::mt19937>
    static Numeric RandomValue(Numeric from, Numeric to)
    {
        thread_local static Generator gen(random_device{}());
        using dist_type = typename conditional<is_integral<Numeric>::value, uniform_int_distribution<Numeric>, uniform_real_distribution<Numeric>>::type;
        thread_local static dist_type dist;
        return dist(gen, typename dist_type::param_type{ from, to });
    }

    template <>
    static char RandomValue(char minimum, char maxmum)
    {
        return static_cast<char>(RandomValue<int>(minimum, maxmum));
    };

    template <>
    static wchar_t RandomValue(wchar_t minimum, wchar_t maxmum)
    {
        return static_cast<wchar_t>(RandomValue<long>(minimum, maxmum));
    };
};

static void CsvHelperTest()
{
    locale::global(locale(""));

    //auto t = make_tuple(10, wstring(L"Bob"), Staff::Kind::Kind1);
    //auto csvText = ToCsv(t);
    //auto s = make_from_tuple<Staff>(move(t));
    //auto size = get_size(t);

#ifdef _DEBUG
    Shos::CsvHelper::Test();
#endif // _DEBUG

    StaffList staffList = {
        Staff(1, L"山田,雅代", Staff::Kind::Kind1, 152.3, 51.03f),
        Staff(2, L"山本\n雅史", Staff::Kind::Kind2, 173.0, 65.84f),
        Staff(5, L"\"山岸雅子\"", Staff::Kind::Kind2, 160.7, 58.61f),
        Staff(3, L"山村\",\"正明", Staff::Kind::Kind2, 168.9, 62.76f),
        Staff(4, L"山里 正孝", Staff::Kind::Kind1, 180.4, 71.6f)
    };

    staffList.Save();
    staffList.Load();
}

int main()
{
    CsvHelperTest();
    ReadWritePerformanceTest test;
    test.Save();
    test.Load();
}
