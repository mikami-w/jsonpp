//
// Created on 2025/10/26.
//

#ifndef JSONPP_PRINT_H
#define JSONPP_PRINT_H
#include <iostream>
#include <map>

namespace MyPrint
{
    namespace details
    {
        inline constexpr char COMMA[] = ",";
        inline constexpr char NEW_LINE[] = "\n";


        template<typename T>
        concept Iterable = requires(T a) {
            { std::begin(a) } -> std::input_or_output_iterator;
            { std::end(a) } -> std::input_or_output_iterator;
        };

        template<typename T>
        concept PlainPrintable = requires(T a) { std::declval<std::ostream>() << a; };

        template<PlainPrintable T>
            requires (!Iterable<T>)
        void _print(T const& value)
        {
            std::cout << value;
        }

        template<typename T, typename U>
        void _print(std::pair<T, U> const& p)
        {
            std::cout << '(';
            _print(p.first);
            std::cout << ',';
            _print(p.second);
            std::cout << ')';
        }

        template<Iterable T>
        void _print(T const& value)
        {
            std::cout << "{";
            for (auto& item: value)
            {
                _print(item);
                std::cout << ",";
            }
            std::cout << char(8) << '}';
        }
    } // namespace details

    template <const char* sep = details::COMMA, const char* end = details::NEW_LINE>
    void print()
    {
        std::cout << end;
    }

    template <const char* sep = details::COMMA, const char* end = details::NEW_LINE, typename T, typename... Args>
    void print(T const& val, Args... args)
    {
        details::_print(val);
        if (sizeof...(Args) != 0)
            std::cout << sep;
        print<sep, end>(args...);
    }
}

inline void test_print_h()
{
    using MyPrint::print;

    print(1, 3.5, "hello"); // 在C++标准中，用作非类型模板参数的指针（如 const char*）必须指向具有**外部链接（external linkage）**的对象。

    std::vector<int> v{1,2,3,4,5,6,7,8,9};
    print(v);

    std::map<int, int> m{{1,2}, {3,4}, {5,6}};
    print(m);

    print();
    // constexpr char sep[] = "||";
    // print<sep,sep>(1, 3.5, "hello", v, m);


}

#endif //JSONPP_PRINT_H
