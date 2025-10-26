//
// Created on 2025/10/25.
//

#include "jsonpp.h"

#include <charconv>
#include <stdexcept>
#ifndef NDEBUG
#include <iostream>
using std::cerr, std::endl;
#endif

namespace JSONpp
{
    /*
     * JSON exceptions
     */
    class JSONParseError: public std::runtime_error
    {
    public:
        static constexpr const char* UNPARSABLE_MESSAGE = "Unparsable character(s)";
        JSONParseError(std::string const& msg, size_t pos):
            std::runtime_error(msg + " at position " + std::to_string(pos)) {}
    };

    class JSONTypeError: public std::runtime_error
    {
    public:
        JSONTypeError(std::string const& msg):
            std::runtime_error(msg) {}
    };
    /*
     * end JSON exceptions
     */

    /*
     * JSON Parser
     */
    class Parser
    {
        size_t pos;
        std::string_view doc;

        static bool is_whitespace(char ch);

        void skip_whitespace(); // 跳过从 pos 开始的空白字符, 使 pos 指向调用函数后的第一个非空白字符
        char ahead() { return doc[pos++]; } // 返回当前字符后前进 1 字符

        JSONValue parse_value();

        JSONValue parse_null();
        JSONValue parse_true();
        JSONValue parse_false();
        JSONValue parse_number();
        JSONValue parse_string(){};
        JSONValue parse_array(){};
        JSONValue parse_object(){};

    public:
        Parser() = delete;
        explicit Parser(std::string_view in): pos(0), doc(in) {}

        JSONValue parse();

    };

    bool Parser::is_whitespace(char ch)
    {
        static const char ws[]{' ', '\n', '\r', '\t'};
        return ch == ws[0] || ch == ws[1] || ch == ws[2] || ch == ws[3];
    }

    void Parser::skip_whitespace()
    {
        if (pos >= doc.size())
            return; // 到达文档尽头

        if (!is_whitespace(doc[pos]))
            return;
        while (is_whitespace(doc[++pos]));
    }

    JSONValue Parser::parse_value()
    { // 调用该函数之前与之后均调用了 skip_whitespace()
        char ch = doc[pos];
        switch (ch)
        {
        case 'n':
            return parse_null();
        case 't':
            return parse_true();
        case 'f':
            return parse_false();
        case '\"':
            return parse_string();
        case '[':
            return parse_array();
        case '{':
            return parse_object();
        default:
            if ((ch >= '0' && ch <= '9') || ch == '-')
                return parse_number();
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, pos);
        }
    }

    JSONValue Parser::parse_null()
    {
        if (doc.substr(pos, 4) != "null")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, pos);
        pos += 4;
        return {};
    }

    JSONValue Parser::parse_true()
    {
        if (doc.substr(pos, 4) != "true")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, pos);
        pos += 4;
        return JSONValue(true);
    }

    JSONValue Parser::parse_false()
    {
        if (doc.substr(pos, 5) != "false")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, pos);
        pos += 5;
        return JSONValue(false);
    }

    JSONValue Parser::parse_number()
    {
        size_t start = pos;
        while (isdigit(doc[pos])
            || doc[pos] == '.'
            || doc[pos] == '-'
            || doc[pos] == '+'
            || doc[pos] == 'e'
            || doc[pos] == 'E')
            pos++; // 停在第 1 个不可能是数字字符的位置

        auto num = doc.substr(start, pos - start);
        std::int64_t val_i{};
        auto res_i = std::from_chars(num.data(), num.data() + num.size(), val_i);
        if (res_i.ptr == num.data() + num.size() && res_i.ec == std::errc()) // 成功
        {
            return {val_i};
        }

        double val_f{};
        auto res_f = std::from_chars(num.data(), num.data() + num.size(), val_f);
        if (res_f.ptr == num.data() + num.size() && res_f.ec == std::errc()) // 成功
        {
            return {val_f};
        }

        if (res_f.ec == std::errc::result_out_of_range)
            throw JSONParseError("Number is out of range", start);
        else
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, res_f.ptr - num.data());
    }

    JSONValue Parser::parse()
    {
        skip_whitespace();
        auto val = parse_value();
        skip_whitespace();

        // pos == doc.size() 表示恰好解析整个文档
        if (pos < doc.size())
            throw JSONParseError("Unexpected character(s) after JSON value", pos);
#ifndef NDEBUG
        if (pos > doc.size())
            throw JSONParseError("WTF pos is beyond size of doc " + std::to_string(doc.size()), pos);
#endif

        return val;
    }
    /*
     * end JSON Parser
     */

    /*
     * asserted accessor
     */
    using _J_Tp = std::variant
        <
            JNull,
            bool,
            std::int64_t,
            double,
            std::string,
            JArray,
            JObject
        >;

    template<typename T>
    static T& as_impl(_J_Tp& v, const char* typeName)
    {
        try
        {
            return std::get<T>(v);
        } catch (const std::bad_variant_access&)
        {
            throw JSONTypeError(std::string("Value is not a ") + typeName);
        }
    }

    template<typename T>
    static T const& as_impl(const _J_Tp& v, const char* typeName)
    {
        try
        {
            return std::get<T>(v);
        } catch (const std::bad_variant_access&)
        {
            throw JSONTypeError(std::string("Value is not a ") + typeName);
        }
    }

    bool JSONValue::as_bool() const
    {
        return as_impl<bool>(value, "bool");
    }

    std::int64_t JSONValue::as_int64() const
    {
        return as_impl<std::int64_t>(value, "int64");
    }

    double JSONValue::as_double() const
    {
        return as_impl<double>(value, "double");
    }

    std::string const& JSONValue::as_string() const
    {
        return as_impl<std::string>(value, "string");
    }

    JArray const& JSONValue::as_array() const
    {
        return as_impl<JArray>(value, "array");
    }

    JObject const& JSONValue::as_object() const
    {
        return as_impl<JObject>(value, "object");
    }

    bool& JSONValue::as_bool()
    {
        return as_impl<bool>(value, "bool");
    }

    std::int64_t& JSONValue::as_int64()
    {
        return as_impl<std::int64_t>(value, "int64");
    }

    double& JSONValue::as_double()
    {
        return as_impl<double>(value, "double");
    }

    std::string& JSONValue::as_string()
    {
        return as_impl<std::string>(value, "string");
    }

    JArray& JSONValue::as_array()
    {
        return as_impl<JArray>(value, "array");
    }

    JObject& JSONValue::as_object()
    {
        return as_impl<JObject>(value, "object");
    }

    /*
     * endasserted accessor
     */

    JSONValue parse(std::string_view json_str)
    {
        return Parser(json_str).parse();
    }
}
