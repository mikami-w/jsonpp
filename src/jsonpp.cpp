#include "jsonpp.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include "parser.hpp"
#include "stream_traits.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"
#include "serializer.hpp"


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
    std::string json::stringify() const
    {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
    json parse(std::string_view json_str)
    {
        StringViewStream stream(json_str);
        return Parser<StringViewStream>(stream).parse();
    }
}
