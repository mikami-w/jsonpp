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
#include <iostream>
#include <sstream>
using std::cerr, std::endl;
#endif

namespace JSONpp
{
    /*
     * JSONStringParser
     */
    template<typename StreamT>
    class JSONStringParser
    {
        static_assert(isJsonStream_v<StreamT>, "StreamT should be a JSON Stream.");

        StreamT& m_stream;

        char peek() const { return m_stream.peek(); }
        char advance() { return m_stream.advance(); }
        size_t tell_pos() const { return m_stream.tell_pos(); }

        template<std::enable_if_t<isSizedStream_v<StreamT>, int> = 0>
        size_t size() const { return m_stream.size(); }

        template<std::enable_if_t<isSeekableStream_v<StreamT>, int> = 0>
        char seek(size_t step) { return m_stream.seek(step); }

        bool end() const;

        struct hex4_result
        {
            std::int16_t number = 0;
            bool error_occurred = false;
        };

        static hex4_result parse_hex4(std::string_view num);

    public:
        JSONStringParser(StreamT& stream): m_stream(stream) {}
        std::string parse();

    };

    template <typename StreamT>
    bool JSONStringParser<StreamT>::end() const
    {
        if constexpr (isSeekableStream_v<StreamT>)
        {
#ifndef NDEBUG
            assert(m_stream.tell_pos() <= m_stream.size(), "WTF pos > m_stream.size()");
#endif
            return m_stream.tell_pos() == m_stream.size();
        }
        else
        { // TODO: 这玩意怎么写?
            return m_stream.eof();
        }
    }

    template<typename StreamT>
    typename JSONStringParser<StreamT>::hex4_result JSONStringParser<StreamT>::parse_hex4(std::string_view num)
    {
        std::int16_t result;
        auto [ptr, ec] = std::from_chars(num.data(), num.data() + 4, result, 16);
        if (ec == std::errc() && ptr == num.data() + num.size())
            return {result, false};
        return {0, true};
    }

    template<typename StreamT>
    std::string JSONStringParser<StreamT>::parse()
    { // TODO: 处理转义字符
        size_t const strBegin = tell_pos(); // 字符串起点, 跳过左引号
        advance();
        size_t chunkBegin = tell_pos();

        std::string str;
        if constexpr (isSizedStream_v<StreamT>)
        { // 如果能直接得到整个流的大小则直接预留空间
            str.reserve(size());
        }

        // TODO: 分别为随机访问流和顺序流编写parse代码(前者分块写入, 后者逐字符)
        bool isEscaping = false;
        while (!end() && (peek() != '\"' || (peek() == '\"' && isEscaping)))
        {
            // JSON 规范 (RFC 8259) 禁止未转义的控制字符 (U+0000 到 U+001F)
            if (peek() < 0x20)
                throw JSONParseError("Unescaped control character in string", tell_pos());

            if (isEscaping)
            {
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
                        char buf[5]{};
                        m_stream.read(buf, 4);
                        auto [code, err] = parse_hex4(buf);
                        if (err)
                            throw JSONParseError("Invalid hexadecimal digits found in Unicode escape sequence", tell_pos());
                        seek(4);
                        break;
                    }
                default:
                    throw JSONParseError("Invalid escape character", tell_pos());
                }

                if constexpr (isSeekableStream_v<StreamT>)
                { // 可随机访问流
                    chunkBegin = tell_pos();
                }
                // 顺序流无需记录块起始位置, 因为是逐字节处理
                isEscaping = false;
                continue;
            }

            if (peek() == '\\' && !isEscaping)
            {
                isEscaping = true;
                if constexpr (isSeekableStream_v<StreamT>)
                { // 可随机访问流
                    size_t chunkLength = tell_pos() - chunkBegin;
                    str.append(m_stream.get_chunk(chunkBegin, chunkBegin + chunkLength));
                }
                // 顺序流只需跳过'\\'
                advance();
                continue;
            }

            if constexpr (isSeekableStream_v<StreamT>)
            {
                // 普通字符, 指针前进
                advance();
            }
            else
            {
                // 顺序流, 逐字节处理
                str.push_back(advance());
            }

        }

        if (peek() == '\"')
        {
            if constexpr (isSeekableStream_v<Stream>)
            {
                // 字符串结束, 写入最后一个块
                size_t chunkLength = tell_pos() - chunkBegin;
                str.append(m_stream.get_chunk(chunkBegin, chunkBegin + chunkLength));
            }
        }
        else if (end())
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
    class Parser
    {
        static_assert(isJsonStream_v<StreamT>, "StreamT should be a JSON Stream.");

        size_t pos;
        std::string_view doc;

        char peek() const { return doc[pos]; }
        char advance() { return doc[pos++]; }
        char advance(size_t step) { pos += step; return doc[pos]; }
        size_t tell_pos() const { return pos; }

        static bool is_whitespace(char ch);

        void skip_whitespace(); // 跳过从 pos 开始的空白字符, 使 pos 指向调用函数后的第一个非空白字符

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

        explicit Parser(std::string_view in) : pos(0), doc(in) {}

        std::optional<JSONValue> parse();
    };

    template<typename StreamT>
    bool Parser<StreamT>::is_whitespace(char ch)
    {
        return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
    }

    template<typename StreamT>
    void Parser<StreamT>::skip_whitespace()
    {
        if (tell_pos() >= doc.size())
            return; // 到达文档尽头

        if (!is_whitespace(peek()))
            return;
        while (is_whitespace((advance(), peek())));
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
        if (doc.substr(tell_pos(), 4) != "null")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
        advance(4);
        return {};
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_true()
    {
        if (doc.substr(tell_pos(), 4) != "true")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
    advance(4);
        return JSONValue(true);
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_false()
    {
        if (doc.substr(tell_pos(), 5) != "false")
            throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
        advance(5);
        return JSONValue(false);
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_number()
    {
        size_t start = tell_pos();
        while (isdigit(peek())
            || peek() == '.'
            || peek() == '-'
            || peek() == '+'
            || peek() == 'e'
            || peek() == 'E')
            advance(); // 停在第 1 个不可能是数字字符的位置

        auto num = doc.substr(start, tell_pos() - start);
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

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_string()
    {
        return JSONStringParser<StreamT>(doc, pos).parse();
    }

    template<typename StreamT>
    JSONValue Parser<StreamT>::parse_array()
    {
        JArray arr;
        auto start = tell_pos(); // 跳过左 [
        advance();
        skip_whitespace();
        while (tell_pos() < doc.size() && peek() != ']')
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
        while (tell_pos() < doc.size() && peek() != '}')
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
        if (tell_pos() == doc.size()) // doc 为空
            return std::nullopt;

        auto val = parse_value();
        skip_whitespace();

        if (tell_pos() == doc.size()) // 表示恰好解析整个文档
            return val;

        if (tell_pos() < doc.size())
            throw JSONParseError("Unexpected character(s) after JSON value");
#ifndef NDEBUG
        if (tell_pos() > doc.size())
            throw JSONParseError("WTF tell_pos() is beyond size of doc " + std::to_string(doc.size()), tell_pos());
#endif
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
        return Parser(json_str).parse();
    }
}
