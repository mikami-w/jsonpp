#include "jsonpp.hpp"

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <charconv>
#include "parser.hpp"
#include "stream_traits.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"
#include "serializer.hpp"


#ifndef NDEBUG
#include <cassert>
#include <iostream>
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

    void json::dump(std::string& buffer) const
    {
        std::visit(
            [&buffer](auto&& v)
            {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, std::monostate>)
                    return; // empty json
                if constexpr (std::is_same_v<T, null>)
                    buffer.append("null", 4);
                if constexpr (std::is_same_v<T, bool>)
                {
                    if (v) buffer.append("true", 4);
                    else buffer.append("false", 5);
                }
                if constexpr (std::is_same_v<T, std::int64_t>)
                {
                    constexpr size_t MAX_INT_CHARS = 32;
                    char cbuf[MAX_INT_CHARS];
                    auto result = std::to_chars(cbuf, cbuf + sizeof(cbuf), v);
                    buffer.append(cbuf, result.ptr - cbuf);
                }
                if constexpr (std::is_same_v<T, double>)
                {
                    constexpr size_t MAX_DOUBLE_CHARS = 64;
                    char cbuf[MAX_DOUBLE_CHARS]; // double 精度高时可能需要更多空间，建议给大一点
                    auto result = std::to_chars(cbuf, cbuf + sizeof(cbuf), v, std::chars_format::general);
                    buffer.append(cbuf, result.ptr - cbuf);
                }
                if constexpr (std::is_same_v<T, std::string>)
                {
                    escape_string(buffer, v);
                }
                if constexpr (std::is_same_v<T, array>)
                {
                    bool first = true;
                    // os << '[';
                    buffer.push_back('[');
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
                            // os << item;
                            item.dump(buffer);
                        }
                        else
                        {
                            buffer.push_back(',');
                            item.dump(buffer);
                        }
                    }
                    buffer.push_back(']');
                }
                if constexpr (std::is_same_v<T, object>)
                {
                    bool first = true;
                    buffer.push_back('{');
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
                            // os << json(item.first) << ':' << item.second;
                            json(item.first).dump(buffer);
                            buffer.push_back(':');
                            item.second.dump(buffer);
                        }
                        else
                        {
                            buffer.push_back(',');
                            json(item.first).dump(buffer);
                            buffer.push_back(':');
                            item.second.dump(buffer);
                        }
                    }
                    buffer.push_back('}');
                }
            }, this->value
        );

    }

    /*
     * end asserted accessor
     */

    json parse(std::string_view json_str)
    {
        StringViewStream stream(json_str);
        return Parser<StringViewStream>(stream).parse();
    }

    json parse(std::istream& json_istream)
    {
        IStreamStream stream(json_istream);
        return Parser<IStreamStream>(stream).parse();
    }
}
