#ifndef JSONPP_PARSER_H
#define JSONPP_PARSER_H

#include "stream_traits.hpp"
#include "jsonexception.hpp"

#include <exception>
#include <string_view>
#include <charconv>
#include <cstddef>

#include "jsonpp.hpp"

#define JSONPP_IMPORT_PARSERBASE_MEMBERS_ \
using ParserBase<StreamT>::m_stream;        \
using ParserBase<StreamT>::peek;            \
using ParserBase<StreamT>::advance;         \
using ParserBase<StreamT>::tell_pos;        \
using ParserBase<StreamT>::eof;             \
using ParserBase<StreamT>::consume;         \
using ParserBase<StreamT>::size;            \
using ParserBase<StreamT>::seek;            \
using ParserBase<StreamT>::get_chunk;       \
using ParserBase<StreamT>::read_chunk_until;

#define JSONPP_CHECK_EOF_() \
do { \
if (eof()) \
throw JSONParseError(JSONParseError::UNEXPECTED_EOF_MESSAGE); \
} while(0)

namespace JSONpp
{
    using namespace traits;

    /*
     * ParserBase
     */
    template <typename StreamT>
    class ParserBase
    {
        static_assert(isJsonStream_v<StreamT>, "StreamT should be a JSON Stream.");

    protected:
        StreamT& m_stream;

    public:
        char peek() const noexcept { return m_stream.peek(); }
        char advance() noexcept { return m_stream.advance(); }
        std::size_t tell_pos() const noexcept { return m_stream.tell_pos(); }
        bool eof() const noexcept { return m_stream.eof(); }
        void consume(char expected, std::exception const& e) { if (advance() == expected); else throw e; }

        std::size_t size() const { if constexpr (isSizedStream_v<StreamT>) { return m_stream.size(); }
            else { static_assert(false, ".size() was called, but the stream is not a Sized Stream."); } }

        void seek(std::size_t step) { if constexpr (isSeekableStream_v<StreamT>) { m_stream.seek(step); }
            else { static_assert(false, ".seek() was called, but the stream is not a Seekable Stream."); } }

        std::string_view get_chunk(std::size_t begin, std::size_t length) { if constexpr (isContiguousStream_v<StreamT>) { return m_stream.get_chunk(begin, length); }
            else { static_assert(false, ".get_chunk() was called, but the stream is not a Contiguous Stream."); } }

        template <typename FunctorT>
        std::string_view read_chunk_until(FunctorT predicate) { if constexpr (isContiguousStream_v<StreamT>) { return m_stream.read_chunk_until(predicate); }
            else { static_assert(false, ".get_chunk_until() was called, but the stream is not a Contiguous Stream."); } }

        explicit ParserBase(StreamT& stream): m_stream(stream) {}
    };
    /*
     * end ParserBase
     */

    /*
     * JSONStringParser
     */
    template <typename StreamT>
    class JSONStringParser : public ParserBase<StreamT>
    {
    protected:
        JSONPP_IMPORT_PARSERBASE_MEMBERS_

    private:
        std::string result;

        enum class UCPStatus: std::uint8_t // Unicode Code Point Status
        {
            SINGLE,
            HIGH,
            LOW
        };

        struct hex4_result
        {
            std::uint16_t value = 0;
            UCPStatus type = UCPStatus::SINGLE;
        };

        hex4_result read_hex4(std::size_t upos);
        void append_utf8(std::uint32_t codepoint);
        static std::uint32_t get_codepoint(std::uint16_t high, std::uint16_t low);
        void unescape_character();

    public:
        JSONStringParser(StreamT& stream): ParserBase<StreamT>(stream), result() {}
        std::string parse();

    };

    template <typename StreamT>
    typename JSONStringParser<StreamT>::hex4_result JSONStringParser<StreamT>::read_hex4(std::size_t upos)
    {
        std::uint16_t value;
        char num_buf[4];
        for (int i = 0; i < 4; ++i)
            num_buf[i] = advance();
        JSONPP_CHECK_EOF_();

        auto [ptr, ec] = std::from_chars(num_buf, num_buf + 4, value, 16);
        if (ec == std::errc() && ptr == num_buf + 4)
        {
            UCPStatus type;
            if (value >= 0xD800 && value <= 0xDBFF)
                type = UCPStatus::HIGH;
            else if (value >= 0xDC00 && value <= 0xDFFF)
                type = UCPStatus::LOW;
            else
                type = UCPStatus::SINGLE;
            return {value, type};
        }
        else throw JSONParseError("Invalid hexadecimal digits found in Unicode escape sequence", upos);
    }

    template <typename StreamT>
    void JSONStringParser<StreamT>::append_utf8(std::uint32_t codepoint)
    {
        if (codepoint <= 0x7F)
        {
            // ASCII, 0b0xxxxxxx
            result += static_cast<char>(codepoint);
        }
        else if (codepoint <= 0x7FF)
        {
            // 2-byte UTF-8, 0b110xxxxx 0b10xxxxxx
            result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
            result += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0xFFFF)
        {
            // 3-byte UTF-8, 0b1110xxxx 0b10xxxxxx 0b10xxxxxx
            result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
            result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0x10FFFF)
        {
            // 4-byte UTF-8, 0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx
            result += static_cast<char>(0xF0 | (codepoint >> 18));
            result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
    }

    template <typename StreamT>
    std::uint32_t JSONStringParser<StreamT>::get_codepoint(std::uint16_t high, std::uint16_t low)
    {
        return
            0x10000 | (((std::uint32_t)high & 0x03FF) << 10) | ((std::uint32_t)low & 0x03FF);
    }

    template <typename StreamT>
    void JSONStringParser<StreamT>::unescape_character()
    {
        switch (peek())
        {
        case '\"': result += '\"'; advance(); break;
        case '\\': result += '\\'; advance(); break;
        case '/': result += '/'; advance(); break;
        case 'b': result += '\b'; advance(); break;
        case 'f': result += '\f'; advance(); break;
        case 'n': result += '\n'; advance(); break;
        case 'r': result += '\r'; advance(); break;
        case 't': result += '\t'; advance(); break;
        case 'u':
            {
                auto upos = tell_pos();
                advance();

                auto [cp, type] = read_hex4(upos);

                switch (type)
                {
                case UCPStatus::SINGLE:
                    append_utf8(cp);
                    break;
                case UCPStatus::HIGH:
                    {
                        consume('\\', JSONParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos()));
                        consume('u', JSONParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos()));
                        auto [cp_low, type_low] = read_hex4(upos);
                        if (type_low != UCPStatus::LOW)
                            throw JSONParseError("Expected low surrogate after high surrogate in Unicode escape sequence", tell_pos());

                        append_utf8(get_codepoint(cp, cp_low));
                        break;
                    }
                case UCPStatus::LOW:
                    throw JSONParseError("Unexpected low surrogate without preceding high surrogate", upos);
                }
                break;
            }
        default:
            throw JSONParseError("Invalid escape character", tell_pos());
        }
    }

    template <typename StreamT>
    std::string JSONStringParser<StreamT>::parse()
    {
        std::size_t const strBegin = tell_pos(); // 字符串起点, 跳过左引号
        advance();

        if constexpr (isSizedStream_v<StreamT>)
        { // 如果能直接得到整个流的大小则直接预留空间
            result.reserve(size());
        }

        // TODO: 顺序流parse代码待测试(后者逐字符)
        while (!eof())
        {
            if constexpr (isContiguousStream_v<StreamT>)
            {
                std::string_view chunk = read_chunk_until([](char c)
                { // 需要终止搜索的字符
                    return c == '\\' || c == '\"' || static_cast<unsigned char>(c) < 0x20;
                });

                if (!chunk.empty())
                    result.append(chunk);
                // 下面检查为什么停下
            }

            auto ch = peek();
            if (ch == '\"')
                break;

            if (ch == '\\')
            {
                advance();
                unescape_character();
            }
            // JSON 规范 (RFC 8259) 禁止未转义的控制字符 (U+0000 到 U+001F)
            else if (ch < 0x20)
                throw JSONParseError("Unescaped control character in string", tell_pos());
            else
            {
                // 只有 IStreamStream 会在这里命中好字符, StringViewStream 已经在 chunk 中处理了它们
                result += ch;
            }
        }

        if (eof())
            throw JSONParseError("Cannot find end of string, which start at position " + std::to_string(strBegin));

        advance(); // 跳过右引号
        return std::move(result);
    }
    /*
     * end JSONStringParser
     */

    /*
     * JSON Parser
     */
    template <typename StreamT>
    class Parser : public ParserBase<StreamT>
    {
    protected:
        JSONPP_IMPORT_PARSERBASE_MEMBERS_

    private:

        static bool is_whitespace(char ch) noexcept;

        void skip_whitespace() noexcept; // 跳过从 pos 开始的空白字符, 使 pos 指向调用函数后的第一个非空白字符

        void parse_literal(char const* lit, std::size_t len);

        static json parse_number_from_chunk(std::string_view chunk, std::size_t start);

        json parse_value(); // 解析, 返回并跳过从当前 pos 开始的一个 json, 使 pos 指向被解析的 json 后的第一个字节

        json parse_null();
        json parse_true();
        json parse_false();
        json parse_number();
        json parse_string();
        json parse_array();
        json parse_object();

    public:
        Parser() = delete;

        explicit Parser(StreamT& stream) : ParserBase<StreamT>(stream) {}

        json parse();
    };

    template <typename StreamT>
    void Parser<StreamT>::parse_literal(char const* lit, std::size_t len)
    {
        for (std::size_t i = 0; i < len; ++i)
        {
            if (advance() != lit[i])
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos() - 1);
        }
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_number_from_chunk(std::string_view chunk, std::size_t start)
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

    template <typename StreamT>
    bool Parser<StreamT>::is_whitespace(char ch) noexcept
    {
        return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
    }

    template <typename StreamT>
    void Parser<StreamT>::skip_whitespace() noexcept
    {
        while (is_whitespace(peek()))
            advance();
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_value()
    {
        // 调用该函数之前与之后均调用了 skip_whitespace()
        if (eof())
            throw JSONParseError("Unexpected end of file");
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

    template <typename StreamT>
    json Parser<StreamT>::parse_null()
    {
        parse_literal("null", 4);
        return {null()};
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_true()
    {
        parse_literal("true", 4);
        return json(true);
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_false()
    {
        parse_literal("false", 5);
        return json(false);
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_number()
    {
        auto is_num_char = [](char c) -> bool {
            return isdigit(static_cast<unsigned char>(c))
                || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E';
        };

        std::size_t start = tell_pos();

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

    template <typename StreamT>
    json Parser<StreamT>::parse_string()
    {
        return JSONStringParser<StreamT>(m_stream).parse();
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_array()
    {
        array arr;
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
            {
                JSONPP_CHECK_EOF_();
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            }
            advance(); // 跳过 ','

            skip_whitespace();

            if (peek() == ']')
                throw JSONParseError("Expected value after comma, but found ']' instead", tell_pos());
        }
        JSONPP_CHECK_EOF_();

        if (advance() != ']') // 跳过右 ]
            throw JSONParseError("Cannot find the end of array, which start at position " + std::to_string(start));

        return {arr};
    }

    template <typename StreamT>
    json Parser<StreamT>::parse_object()
    {
        object obj;
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
            {
                JSONPP_CHECK_EOF_();
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            }
            advance();

            skip_whitespace();
            auto val = parse_value();
            obj[key.as_string()] = val;

            skip_whitespace();

            if (peek() == '}')
                break;
            else if (peek() != ',')
            {
                JSONPP_CHECK_EOF_();
                throw JSONParseError(JSONParseError::UNPARSABLE_MESSAGE, tell_pos());
            }
            advance(); // skip comma

            skip_whitespace();

            if (peek() == '}')
                throw JSONParseError("Expected value after comma, but found '}' instead", tell_pos());
        }
        JSONPP_CHECK_EOF_();

        if (advance() != '}')
            throw JSONParseError("Cannot find the end of object, which start at position " + std::to_string(start));

        return {obj};
    }

    template <typename StreamT>
    json Parser<StreamT>::parse()
    {
        skip_whitespace();
        if (eof()) // doc 为空
            return {};

        auto val = parse_value();
        skip_whitespace();

        if (eof()) // 表示恰好解析整个文档
            return val;
        else
            throw JSONParseError("Unexpected character(s) after JSON value");

        return {};
    }
    /*
     * end JSON Parser
     */
}

#endif //JSONPP_PARSER_H