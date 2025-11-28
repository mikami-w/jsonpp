#include "jsonpp.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include "parser.hpp"
#include "stream_traits.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"


#ifndef NDEBUG
#include <cassert>
#include <iostream>
#include <sstream>
using std::cerr, std::endl;
#endif


namespace JSONpp
{
    using namespace traits;

    /*
     * asserted accessor
     */
    using JsonType = std::variant
    <
        std::monostate,
        null,
        bool,
        std::int64_t,
        double,
        std::string,
        array,
        object
    >;

    template <typename T>
    static T& as_impl(JsonType& v, char const* typeName)
    {
        try
        {
            return std::get<T>(v);
        }
        catch (const std::bad_variant_access&)
        {
            throw JSONTypeError(std::string("Value is not a ") + typeName);
        }
    }

    template <typename T>
    static T const& as_impl(JsonType const& v, char const* typeName)
    {
        try
        {
            return std::get<T>(v);
        }
        catch (const std::bad_variant_access&)
        {
            throw JSONTypeError(std::string("Value is not a ") + typeName);
        }
    }

    bool json::as_bool() const
    {
        return as_impl<bool>(value, "bool");
    }

    std::int64_t json::as_int() const
    {
        return as_impl<std::int64_t>(value, "int64");
    }

    double json::as_float() const
    {
        return as_impl<double>(value, "double");
    }

    std::string const& json::as_string() const
    {
        return as_impl<std::string>(value, "string");
    }

    array const& json::as_array() const
    {
        return as_impl<array>(value, "array");
    }

    object const& json::as_object() const
    {
        return as_impl<object>(value, "object");
    }

    bool& json::as_bool()
    {
        return as_impl<bool>(value, "bool");
    }

    std::int64_t& json::as_int()
    {
        return as_impl<std::int64_t>(value, "int64");
    }

    double& json::as_float()
    {
        return as_impl<double>(value, "double");
    }

    std::string& json::as_string()
    {
        return as_impl<std::string>(value, "string");
    }

    array& json::as_array()
    {
        return as_impl<array>(value, "array");
    }

    object& json::as_object()
    {
        return as_impl<object>(value, "object");
    }
    /*
     * end asserted accessor
     */

    /*
     * stringifier
     */
    inline bool needs_escaping(char ch)
    {
        return ch == '\"' || ch == '\\' || static_cast<unsigned char>(ch) < 0x20
#ifdef ESCAPE_FORWARD_SLASH
        || ch == '/'
#endif
        ;
    }

    std::ostream& escape_string(std::ostream& os, std::string_view str) // escape v.转义 e.g. \ -> \\, " -> \"
    {
        os << '\"';
        auto chunkBegin = str.begin();
        auto const end = str.end();
        while (chunkBegin < end)
        {
            auto badChar = std::find_if(chunkBegin, end, needs_escaping);
            auto chunkLength = badChar - chunkBegin;
            if (chunkLength > 0)
            {
                os.write(std::addressof(*chunkBegin), chunkLength); // 第一个参数应当为指针而不是迭代器, 尽管部分实现中迭代器底层直接使用指针
            }
            chunkBegin = badChar + 1; // 跳过当前已经写入流的块, 等同于 chunkBegin += chunkLength + 1

            // 下面处理转义
            if (badChar == end)
                break;

            char ch = *badChar;
            switch (ch)
            {
            case '\"': os.write("\\\"", 2);
                break;
            case '\\': os.write("\\\\", 2);
                break;
#ifdef ESCAPE_FORWARD_SLASH
            case '/': os.write("\\/", 2); break;
#endif
            case '\b': os.write("\\b", 2);
                break; // \x08
            case '\f': os.write("\\f", 2);
                break; // \x0C
            case '\n': os.write("\\n", 2);
                break; // \x0A
            case '\r': os.write("\\r", 2);
                break; // \x0D
            case '\t': os.write("\\t", 2);
                break; // \x09
            default:
                {
                    char buf[7]{};
                    std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned short>(ch));
                    os.write(buf, 6); // \uXXXX一定是6字符
                    break;
                }
            }
        }

        return os << '\"';
    }

    std::ostream& operator<<(std::ostream& os, json const& val)
    {
        return std::visit(
            [&os](auto&& v) -> std::ostream&
            {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, std::monostate>)
                    return os; // empty json
                if constexpr (std::is_same_v<T, null>)
                    return os << "null";
                if constexpr (std::is_same_v<T, bool>)
                    return os << (v ? "true" : "false");
                if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, double>)
                    return os << v;
                if constexpr (std::is_same_v<T, std::string>)
                {
                    return escape_string(os, v);
                }
                if constexpr (std::is_same_v<T, array>)
                {
                    bool first = true;
                    os << '[';
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
                            os << item;
                        }
                        else os << ',' << item;
                    }
                    return os << ']';
                }
                if constexpr (std::is_same_v<T, object>)
                {
                    bool first = true;
                    os << '{';
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
                            os << json(item.first) << ':' << item.second;
                        }
                        else os << ',' << json(item.first) << ':' << item.second;
                    }
                    return os << '}';
                }
            }, val.value
        );
    }

    /*
     * end stringifier
     */

    std::string json::stringify() const
    {
        std::stringstream ss;
        ss << *this;

        return ss.str();
    }

    json parse(std::string_view json_str)
    {
        StringViewStream stream(json_str);
        return Parser<StringViewStream>(stream).parse();
    }
}
