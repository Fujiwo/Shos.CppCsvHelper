#pragma once
#include <iostream>
#include <functional>
#include <tuple>
#include <vector>
#include <string>
#include <optional>

namespace Shos
{
    using namespace std;

    class CsvHelper
    {
        static const wchar_t comma            = L',';
        static const wchar_t doubleQuoration  = L'\"';
        static const wchar_t newLine          = L'\n';
        static const wchar_t carriageReturn   = L'\r';
        static const wchar_t defaultSeparator = comma;

    public:
#ifdef _DEBUG
        static void Test()
        {
            wstring text10 = L"AABABABA";
            wstring text11 = ReplaceAll(text10, L"AB", L"");
            wstring text12 = ReplaceAll(text10, L"AB", L"EFG");
            wstring text13 = ReplaceAll(text10, L"BAB", L"X");
            wstring text14 = ReplaceAll(text10, L"", L"X");

            wstring text20 = L"";
            wstring text21 = ReplaceAll(text20, L"", L"");
            wstring text22 = ReplaceAll(text20, L"A", L"");
            wstring text23 = ReplaceAll(text20, L"", L"X");

            wstring text30 = L"";
            wstring text31 = CsvEncode(text30, defaultSeparator);

            wstring text40 = L"山田";
            wstring text41 = CsvEncode(text40, defaultSeparator);

            wstring text50 = L"山田,山岸,\",\"";
            wstring text51 = CsvEncode(text50, defaultSeparator);

            wstring text60 = L"\"山田,\"\n\",山岸,\"\"";
            wstring text61 = CsvEncode(text60, defaultSeparator);
/*
1,"山田,雅代",0,152.300000,51.029999
2,"山本
雅史",1,173.000000,65.839996
5,"""山岸雅子""",1,160.700000,58.610001
3,"山村"",""正明",1,168.900000,62.759998
4,山里 正孝,0,180.400000,71.599998
*/
            wstring text70 = L"1,\"山田, 雅代\",0,152.300000,51.029999";
            auto texts71 = SplitCsv(text70, L',');

            wstring text80 = L"2,\"山本\n雅史\",1,173.000000,65.839996";
            auto texts81 = SplitCsv(text80, L',');

            wstring text90 = L"5,\"\"\"山岸雅子\"\"\",1,160.700000,58.610001";
            auto texts91 = SplitCsv(text90, L',');

            wstring text100 = L"3,\"山村\"\", \"\"正明\",1,168.900000,62.759998";
            auto texts101 = SplitCsv(text100, L',');

            wstring text110 = L"4,山里 正孝,0,180.400000,71.599998";
            auto texts111 = SplitCsv(text110, L',');
        }
#endif // _DEBUG

        template <class TElement, class TIterator, typename... Args>
        static void Write(wostream& stream, const TIterator& begin, const TIterator& end, function<tuple<Args...>(const TElement&)> toTuple, wchar_t separator = defaultSeparator)
        {
            for (auto iterator = begin; iterator != end; iterator++) {
                auto tuple = toTuple(*iterator);
                auto csv   = ToCsv(tuple, separator);
                stream << csv << endl;
            }
        }

        template <class TElement, typename... Args>
        static vector<TElement> Read(wistream& stream, function<tuple<Args...>(const vector<wstring>&)> toTuple, wchar_t separator = defaultSeparator)
        {
            wstring csv;
            wstring textLine;
            while (getline(stream, textLine))
                csv += textLine + L'\n';
            auto lines = SplitToLines(csv);

            vector<TElement> elements;
            for (auto line : lines) {
                auto csvTexts = SplitCsv(line, separator);
                auto tuple    = toTuple(csvTexts);
                elements.push_back(make_from_tuple<TElement>(tuple));
            }
            return elements;
        }

        template <class TElement>
        static vector<TElement> Read(wistream& stream, function<TElement(const vector<wstring>&)> toElement, wchar_t separator = defaultSeparator)
        {
            wstring csv;
            wstring textLine;
            while (getline(stream, textLine))
                csv += textLine + L'\n';
            auto lines = SplitToLines(csv);

            vector<TElement> elements;
            for (auto line : lines) {
                auto csvTexts = SplitCsv(line, separator);
                auto element  = toElement(csvTexts);
                elements.push_back(element);
            }
            return elements;
        }

    private:
        static vector<wstring> SplitToLines(wstring csv)
        {
            vector<wstring> lines;
            bool readingDoubleQuotation = false;
            wstring stringBuilder;
            for (auto character : csv) {
                if (character == doubleQuoration) {
                    readingDoubleQuotation = !readingDoubleQuotation;
                } else if (character == newLine || character == carriageReturn) {
                    if (!readingDoubleQuotation) {
                        auto line = stringBuilder;
                        stringBuilder = L"";
                        if (line.length() > 0)
                            lines.push_back(line);
                        continue;
                    }
                }
                stringBuilder += character;
            }
            auto lastLine = stringBuilder;
            if (lastLine.length() > 0)
                lines.push_back(lastLine);

            return lines;
        }

        class CsvValueReader
        {
        public:
            virtual inline optional<wstring> Read(wstring& stringBuilder, wchar_t character, CsvValueReader*& reader, wchar_t separator);

        protected:
            static wstring ToText(wstring& stringBuilder, CsvValueReader*& reader)
            {
                wstring text = stringBuilder;
                stringBuilder = L"";
                Change(reader, new CsvValueReader());
                return text;
            }

            static void Change(CsvValueReader*& reader, CsvValueReader* newReader)
            {
                delete reader;
                reader = newReader;
            }
        };

        class CsvValueInDoubleQuotationReader : public CsvValueReader
        {
            bool readingDoubleQuotation = false;

        public:
            virtual inline optional<wstring> Read(wstring& stringBuilder, wchar_t character, CsvValueReader*& reader, wchar_t separator) override
            {
                if (character == separator) {
                    if (readingDoubleQuotation)
                        return ToText(stringBuilder, reader);
                    stringBuilder += character;
                } else if (character == doubleQuoration) {
                    if (readingDoubleQuotation)
                        stringBuilder += character;
                    readingDoubleQuotation = !readingDoubleQuotation;
                } else {
                    stringBuilder += character;
                }
                return nullopt;
            }
        };

        static vector<wstring> SplitCsv(wstring csv, wchar_t separator)
        {
            vector<wstring> texts;
            wstring stringBuilder;
            CsvValueReader csvValueReader;
            auto reader = new CsvValueReader();
            for (auto character : csv) {
                auto itemText = reader->Read(stringBuilder, character, reader, separator);
                if (itemText)
                    texts.push_back(*itemText);
            }
            texts.push_back(stringBuilder);
            delete reader;
            return texts;
        }

        static wstring CsvEncode(wstring text, wchar_t separator)
        {
            const wstring doubleQuorationText1(1, doubleQuoration);
            const wstring doubleQuorationText2(2, doubleQuoration);

            if (text.find(separator) != wstring::npos || text.find(doubleQuoration) != wstring::npos || text.find(newLine) != wstring::npos || text.find(carriageReturn) != wstring::npos) {
                text = ReplaceAll(text, doubleQuorationText1, doubleQuorationText2);
                text = doubleQuoration + text + doubleQuoration;
            }
            return text;
        }

        static wstring ReplaceAll(wstring text, wstring from, wstring to)
        {
            auto fromLength = from.length();
            if (fromLength > 0) {
                auto toLength = to.length();
                for (size_t position = 0; position != wstring::npos && position < text.length(); ) {
                    if ((position = text.find(from, position)) != wstring::npos) {
                        text = text.replace(position, fromLength, to);
                        position += toLength;
                    }
                }
            }
            return text;
        }

        template <typename T>
        static wstring ToCsvText(T value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(char value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(static_cast<int>(value));
        }

        template <>
        static wstring ToCsvText(wchar_t value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(static_cast<int>(value));
        }

        template <>
        static wstring ToCsvText(unsigned char value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(static_cast<int>(value));
        }

        template <>
        static wstring ToCsvText(short value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(static_cast<int>(value));
        }

        template <>
        static wstring ToCsvText(unsigned short value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(static_cast<int>(value));
        }

        template <>
        static wstring ToCsvText(int value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(unsigned int value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(long value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(unsigned long value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(long long value, [[maybe_unused]]  wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(unsigned long long value, [[maybe_unused]] wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(float value, [[maybe_unused]]  wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(double value, [[maybe_unused]]  wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(long double value, [[maybe_unused]]  wchar_t separator)
        {
            return to_wstring(value);
        }

        template <>
        static wstring ToCsvText(wstring value, [[maybe_unused]] wchar_t separator)
        {
            return CsvEncode(value, separator);
        }

        static wstring Concat(const vector<wstring>& values, wchar_t separator)
        {
            wstring text;
            size_t index = 0;
            for (auto value : values) {
                if (index != 0)
                    text += separator;
                text += value;
                index++;
            }
            return text;
        }

        template <typename T1>
        static wstring ToCsv(tuple<T1> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2>
        static wstring ToCsv(tuple<T1, T2> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3>
        static wstring ToCsv(tuple<T1, T2, T3> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4>
        static wstring ToCsv(tuple<T1, T2, T3, T4> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5>
        static wstring ToCsv(tuple<T1, T2, T3, T4, T5> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator),
                ToCsvText(get<4>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        static wstring ToCsv(tuple<T1, T2, T3, T4, T5, T6> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator),
                ToCsvText(get<4>(tuple), separator),
                ToCsvText(get<5>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        static wstring ToCsv(tuple<T1, T2, T3, T4, T5, T6, T7> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator),
                ToCsvText(get<4>(tuple), separator),
                ToCsvText(get<5>(tuple), separator),
                ToCsvText(get<6>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
        static wstring ToCsv(tuple<T1, T2, T3, T4, T5, T6, T7, T8> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator),
                ToCsvText(get<4>(tuple), separator),
                ToCsvText(get<5>(tuple), separator),
                ToCsvText(get<6>(tuple), separator),
                ToCsvText(get<7>(tuple), separator)
            };
            return Concat(csvs, separator);
        }

        template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
        static wstring ToCsv(tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9> tuple, wchar_t separator)
        {
            const vector<wstring> csvs = {
                ToCsvText(get<0>(tuple), separator),
                ToCsvText(get<1>(tuple), separator),
                ToCsvText(get<2>(tuple), separator),
                ToCsvText(get<3>(tuple), separator),
                ToCsvText(get<4>(tuple), separator),
                ToCsvText(get<5>(tuple), separator),
                ToCsvText(get<6>(tuple), separator),
                ToCsvText(get<7>(tuple), separator),
                ToCsvText(get<8>(tuple), separator)
            };
            return Concat(csvs, separator);
        }
    };

    optional<wstring> CsvHelper::CsvValueReader::Read(wstring& stringBuilder, wchar_t character, CsvValueReader*& reader, wchar_t separator)
    {
        if (character == separator)
            return ToText(stringBuilder, reader);
        if (character == doubleQuoration)
            Change(reader, new CsvValueInDoubleQuotationReader());
        else
            stringBuilder += character;
        return nullopt;
    }
} // namespace Shos
