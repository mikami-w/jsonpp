//
// Created on 2025/10/25.
//

#include "jsonpp.h"

#include <algorithm>
#include <charconv>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include "jsonexception.h"
#include "basic_json_stream.h"

#ifndef NDEBUG
#include <cassert>
#include <iostream>
#include <sstream>
using std::cerr, std::endl;
#endif

namespace JSONpp
{
    /*
     * ParserBase
     */
    template<typename StreamT>
    class ParserBase
    {
        static_assert(isJsonStream_v<StreamT>, "StreamT should be a JSON Stream.");

    public:
        StreamT& m_stream;

        char peek() const { return m_stream.peek(); }
        char advance() { return m_stream.advance(); }
        size_t tell_pos() const { return m_stream.tell_pos(); }

        template<std::enable_if_t<isSizedStream_v<StreamT>, int> = 0>
        size_t size() const { return m_stream.size(); }

        template<std::enable_if_t<isSeekableStream_v<StreamT>, int> = 0>
        void seek(size_t step) { m_stream.seek(step); }

        template<typename = std::enable_if_t<isContiguousStream_v<StreamT>>>
        std::string_view get_chunk(size_t begin, size_t length) { return m_stream.get_chunk(begin, length); }

        bool eof() const;

        explicit ParserBase(StreamT& stream): m_stream(stream) {}
    };

    template <typename StreamT>
    bool ParserBase<StreamT>::eof() const
    {
        if constexpr (isSeekableStream_v<StreamT>)
        {
#ifndef NDEBUG
            assert(m_stream.tell_pos() <= m_stream.size()); //, "WTF pos > m_stream.size()"
#endif
            return m_stream.tell_pos() == m_stream.size();
        }
        else
        { // TODO: 这玩意怎么写?
            return m_stream.eof();
        }
    }

    /*
     * JSONStringParser
     */
    template<typename StreamT>
    class JSONStringParser : public ParserBase<StreamT>
    {
    protected:
        using ParserBase<StreamT>::peek;
        using ParserBase<StreamT>::advance;
        using ParserBase<StreamT>::tell_pos;
        using ParserBase<StreamT>::eof;
        using ParserBase<StreamT>::size;
        using ParserBase<StreamT>::seek;
        using ParserBase<StreamT>::get_chunk;
        using ParserBase<StreamT>::m_stream;

    private:
        struct hex4_result
        {
            std::int16_t number = 0;
            bool error_occurred = false;
        };

        static hex4_result parse_hex4(std::string_view num);
        void parse_escape_characters(std::string& str);

    public:
        JSONStringParser(StreamT& stream): ParserBase<StreamT>(stream) {}
        std::string parse();

    };

    template<typename StreamT>
    typename JSONStringParser<StreamT>::hex4_result JSONStringParser<StreamT>::parse_hex4(std::string_view num)
    {
        std::int16_t result;
        auto [ptr, ec] = std::from_chars(num.data(), num.data() + 4, result, 16);
        if (ec == std::errc() && ptr == num.data() + num.size())
            return {result, false};
        return {0, true};
    }

    template <typename StreamT>
    void JSONStringParser<StreamT>::parse_escape_characters(std::string& str)
    {
        // JSON 规范 (RFC 8259) 禁止未转义的控制字符 (U+0000 到 U+001F)
        if (peek() < 0x20)
            throw JSONParseError("Unescaped control character in string", tell_pos());


        switch (peek())
        {
        case '\"': str.append("\""); advance(); break;
        case '\\': str.append("\\"); advance(); break;
            // #ifdef ESCAPE_FORWARD_SLASH
            //                  case '/': str.append("/"); advance(); break;
            // #endif
        case 'b': str.append("\b"); advance(); break;
        case 'f': str.append("\f"); advance(); break;
        case 'n': str.append("\n"); advance(); break;
        case 'r': str.append("\r"); advance(); break;
        case 't': str.append("\t"); advance(); break;
        case 'u':
            { // TODO: 解析 utf-16 代理, 没做呢
                advance();
                // char buf[5]{};
                if constexpr (isContiguousStream_v<StreamT>)
                {
                    std::string_view buf = m_stream.get_chunk(tell_pos(), 4);
                    auto [code, err] = parse_hex4(buf);
                    if (err)
                        throw JSONParseError("Invalid hexadecimal digits found in Unicode escape sequence", tell_pos());
                    seek(4);
                }
                else
                {
                    std::string buf;
                    buf += advance();
                    buf += advance();
                    buf += advance();
                    buf += advance();
                    auto [code, err] = parse_hex4(buf);
                    if (err)
                        throw JSONParseError("Invalid hexadecimal digits found in Unicode escape sequence", tell_pos());
                }
                break;
            }
        default:
            throw JSONParseError("Invalid escape character", tell_pos());
        }

    }

    template<typename StreamT>
    std::string JSONStringParser<StreamT>::parse()
    { // TODO: 处理转义字符
        size_t const strBegin = tell_pos(); // 字符串起点, 跳过左引号
        advance();

        std::string str;
        if constexpr (isSizedStream_v<StreamT>)
        { // 如果能直接得到整个流的大小则直接预留空间
            str.reserve(size());
        }

        // TODO: 分别为随机访问流和顺序流编写parse代码(前者分块写入, 后者逐字符)
        while (!eof() && (peek() != '\"' || (peek() == '\"')))
        {
        }

        if (eof())
            throw JSONParseError("Cannot find end of string, which start at position " + std::to_string(strBegin));

        advance(); // 跳过右引号
        return str;
    }
    /*
     * end JSONStringParser
     */

    /*
     * JSON Parser
     */
    template<typename StreamT>
    class Parser : public ParserBase<StreamT>
    {
    protected:
        using ParserBase<StreamT>::peek;
        using ParserBase<StreamT>::advance;
        using ParserBase<StreamT>::tell_pos;
        using ParserBase<StreamT>::eof;
        using ParserBase<StreamT>::size;
        using ParserBase<StreamT>::seek;
        using ParserBase<StreamT>::get_chunk;
        using ParserBase<StreamT>::m_stream;

    private:

        static bool is_whitespace(char ch);

        void skip_whitespace(); // 跳过从 pos 开始的空白字符, 使 pos 指向调用函数后的第一个非空白字符

        void parse_literal(char const* lit, size_t len);

        static JSONValue parse_number_from_chunk(std::string_view chunk, size_t start);

        JSONValue parse_value(); // 解析, 返回并跳过从当前 pos 开始的一个 JSONValue, 使 pos 指向被解析的 JSONValue 后的第一个字节

        JSONValue parse_null();
        JSONValue parse_true();
        JSONValue parse_false();
        JSONValue parse_number();
        JSONValue parse_string();
        JSONValue parse_array();
        JSONValue parse_object();

    public:
        Parser() = delete;

        explicit Parser(StreamT& stream) : ParserBase<StreamT>(stream) {}

        std::optional<JSONValue> parse();
    };

    template <typename StreamT>
    void Parser<StreamT>::parse_literal(char const* lit, size_t len)
    {
        for (size_t i = 0; i < len; ++i)
        {
            if (advance() != lit[i])
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos() - 1);
        }
    }

    template <typename StreamT>
    JSONValue Parser<StreamT>::parse_number_from_chunk(std::string_view chunk, size_t start)
    {
        std::int64_t val_i{};
        auto res_i = std::from_chars(chunk.data(), chunk.data() + chunk.size(), val_i);
        if (res_i.ptr == chunk.data() + chunk.size() && res_i.ec == std::errc()) // 成功
        {
            return {val_i};
        }

        double val_f{};
        auto res_f = std::from_chars(chunk.data(), chunk.data() + chunk.size(), val_f);
        if (res_f.ptr == chunk.data() + chunk.size() && res_f.ec == std::errc()) // 成功
        {
            return {val_f};
        }

        if (res_f.ec == std::errc::result_out_of_range)
            throw JSONParseError("Number is out of range", start);
        else
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, res_f.ptr - chunk.data());

    }

    template<typename StreamT>
    bool Parser<StreamT>::is_whitespace(char ch)
    {
        return std::string_view(" \n\r\t").find(ch) != std::string_view::npos;
    }

    template<typename StreamT>
    void Parser<StreamT>::skip_whitespace()
    {
        // TODO: 针对随机访问流进行优化
        while (!eof() && is_whitespace(peek()))
        {
            advance();
        }
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_value()
    {
        // 调用该函数之前与之后均调用了 skip_whitespace()
        char ch = peek();
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
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
        }
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_null()
    {
        parse_literal("null", 4);
        return {};
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_true()
    {
        parse_literal("true", 4);
        return JSONValue(true);
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_false()
    {
        parse_literal("false", 5);
        return JSONValue(false);
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_number()
    {
        auto is_num_char = [](char c) -> bool {
            return isdigit(static_cast<unsigned char>(c))
                || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E';
        };

        size_t start = tell_pos();

        if constexpr (isContiguousStream_v<StreamT>)
        {
            while (is_num_char(peek()))
                advance();
            return parse_number_from_chunk(get_chunk(start, tell_pos() - start), start);
        }
        else
        {
            std::string chunk;
            while (is_num_char(peek()))
            {
                chunk += advance(); // 停在第 1 个不可能是数字字符的位置
            }
            return parse_number_from_chunk(chunk, start);
        }

    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_string()
    {
        return JSONStringParser<StreamT>(m_stream).parse();
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_array()
    {
        JArray arr;
        auto start = tell_pos(); // 跳过左 [
        advance();
        skip_whitespace();
        while (!eof() && peek() != ']')
        {
            arr.push_back(parse_value());
            skip_whitespace();

            // json数组中对象以外的字符只能是空白字符或'['或']'或','
            if (peek() == ']')
                break;
            else if (peek() != ',')
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            advance(); // 跳过 ','

            skip_whitespace();
            if (peek() == ']')
                throw JSONParseError("Expected value after comma, but found ']' instead", tell_pos());
        }
        if (advance() != ']') // 跳过右 ]
            throw JSONParseError("Cannot find the end of array, which start at position " + std::to_string(start));

        return {arr};
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_object()
    {
        JObject obj;
        auto start = tell_pos(); // 跳过左 {
        advance();
        skip_whitespace();
        while (!eof() && peek() != '}')
        {
            auto key = parse_value();

            if (!key.is_string()) [[unlikely]]
                throw JSONTypeError("Key of an object must be string");

            skip_whitespace();
            if (peek() != ':')
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            advance();

            skip_whitespace();
            auto val = parse_value();
            obj[key.as_string()] = val;

            skip_whitespace();
            if (peek() == '}')
                break;
            else if (peek() != ',')
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            advance(); // skip comma

            skip_whitespace();
            if (peek() == '}')
                throw JSONParseError("Expected value after comma, but found '}' instead", tell_pos());
        }
        if (advance() != '}')
            throw JSONParseError("Cannot find the end of object, which start at position ", start);

        return {obj};
    }

    template<typename StreamT>
    std::optional<JSONValue> Parser<StreamT>::parse()
    {
        skip_whitespace();
        if (eof()) // doc 为空
            return std::nullopt;

        auto val = parse_value();
        skip_whitespace();

        if (eof()) // 表示恰好解析整个文档
            return val;
        else
            throw JSONParseError("Unexpected character(s) after JSON value");

        return std::nullopt;
    }

    /*
     * end JSON Parser
     */

    /*
     * asserted accessor
     */
    using _Json_Tp = std::variant
    <
        JNull,
        bool,
        std::int64_t,
        double,
        std::string,
        JArray,
        JObject
    >;

    template <typename T>
    static T& as_impl(_Json_Tp& v, char const* typeName)
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
    static T const& as_impl(const _Json_Tp& v, char const* typeName)
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

    std::ostream& operator<<(std::ostream& os, JSONValue const& val)
    {
        return std::visit(
            [&os](auto&& v) -> std::ostream&
            {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, JNull> || std::is_same_v<T, std::nullptr_t>)
                    return os << "null";
                if constexpr (std::is_same_v<T, bool>)
                    return os << (v ? "true" : "false");
                if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, double>)
                    return os << v;
                if constexpr (std::is_same_v<T, std::string>)
                {
                    return escape_string(os, v);
                }
                if constexpr (std::is_same_v<T, JArray>)
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
                if constexpr (std::is_same_v<T, JObject>)
                {
                    bool first = true;
                    os << '{';
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
                            os << JSONValue(item.first) << ':' << item.second;
                        }
                        else os << ',' << JSONValue(item.first) << ':' << item.second;
                    }
                    return os << '}';
                }
            }, val.value
        );
    }

    /*
     * end stringifier
     */

    std::string JSONValue::stringify() const
    {
        std::stringstream ss;
        ss << *this;

        return ss.str();
    }

    std::optional<JSONValue> parse(std::string_view json_str)
    {
        StringViewStream stream(json_str);
        return Parser<StringViewStream>(stream).parse();
    }
}
