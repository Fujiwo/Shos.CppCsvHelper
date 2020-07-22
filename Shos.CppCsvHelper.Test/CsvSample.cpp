//#include <tuple>
using namespace std;
#include "StaffList.h"

//template <typename... Args>
//size_t get_size(tuple<Args...> t)
//{
//    return tuple_size<tuple<Args...>>();
//}
//
//void f(int value)
//{
//    wcout << L"int: " << value << endl;
//}
//
//
//void f(Staff::Kind value)
//{
//    wcout << L"Staff::Kind: " << (int)value << endl;
//}
//
//void f(wstring value)
//{
//    wcout << L"wstring: " << value << endl;
//}
//
//template <typename... Args>
//void fff(tuple<Args...> t)
//{
//    auto size = tuple_size<tuple<Args...>>();
//    if (size > 0) f(get<0>(t));
//    //if (size > 1) f(get<1>(t));
//    //if (size > 2) f(get<2>(t));
//    //if (size > 3) f(get<3>(t));
//    //if (size > 4) f(get<4>(t));
//    //if (size > 5) f(get<5>(t));
//    //if (size > 6) f(get<6>(t));
//    //if (size > 7) f(get<7>(t));
//    //if (size > 8) f(get<8>(t));
//    //if (size > 9) f(get<9>(t));
//}
//
//
//
////template <>
////void fff<3>(tuple<Args...> t)
////{
////    auto size = tuple_size<tuple<Args...>>();
////    if (size > 0) f(get<0>(t));
////    if (size > 1) f(get<1>(t));
////    if (size > 2) f(get<2>(t));
////}

int main()
{
    locale::global(locale(""));

    //auto t = make_tuple(10, wstring(L"Bob"), Staff::Kind::Kind1);
    //auto csvText = ToCsv(t);
    //auto s = make_from_tuple<Staff>(move(t));
    //auto size = get_size(t);

    Shos::CsvHelper::Test();

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
