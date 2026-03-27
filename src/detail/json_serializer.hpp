/*
jsonpp - A modern, header-only C++ JSON library
Copyright 2025-2026 Mikami (jsonpp project)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#ifndef JSONPP_SERIALIZER_HPP
#define JSONPP_SERIALIZER_HPP

#include "json_fwd.hpp"
#include "json_serialize_handler.hpp"
#include "traits.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <variant>

namespace jsonpp::details
{
    template <typename JsonT, typename SerializeHandlerT>
    class JsonSerializer
    {
        static_assert(traits::is_json_serialize_handler_v<SerializeHandlerT>,
            "SerializeHandlerT should be a JSON Serialize Handler.");

        using boolean = typename JsonT::boolean;
        using number_int = typename JsonT::number_int;
        using number_float = typename JsonT::number_float;
        using string = typename JsonT::string;
        using array = typename JsonT::array;
        using object = typename JsonT::object;

        SerializeHandlerT& m_sh;

        static bool needs_escaping(char ch)
        {
            return ch == '\"' || ch == '\\' || static_cast<unsigned char>(ch) < 0x20
#ifdef ESCAPE_FORWARD_SLASH
            || ch == '/'
#endif
                ;
        }

        // 用于写入换行和当前的缩进
        void append_indent(std::string_view indent, int depth)
        {
            m_sh.append('\n');
            for (int i = 0; i < depth; ++i) {
                m_sh.append(indent);
            }
        }

    public:
        explicit JsonSerializer(SerializeHandlerT& handler) : m_sh(handler) {}

        template <typename StringT>
        void escape_string(StringT const& str) // escape v.转义 e.g. \ -> \\, " -> \"
        {
            std::string_view sv(std::data(str), std::size(str));
            m_sh.append('\"');
            auto chunkBegin = sv.begin();
            auto const end = sv.end();
            while (chunkBegin < end)
            {
                auto badChar = std::find_if(chunkBegin, end, needs_escaping);
                auto chunkLength = badChar - chunkBegin;
                if (chunkLength > 0)
                {
                    m_sh.append(std::addressof(*chunkBegin), chunkLength); // 第一个参数应当为指针而不是迭代器, 尽管部分实现中迭代器底层直接使用指针
                }
                if (badChar == end)
                    break; // 到达字符串结尾, 停止处理
                chunkBegin = badChar + 1; // 跳过当前已经写入流的块, 等同于 chunkBegin += chunkLength + 1

                // 下面处理转义
                char ch = *badChar;
                switch (ch)
                {
                case '\"': m_sh.append("\\\"", 2);
                    break;
                case '\\': m_sh.append("\\\\", 2);
                    break;
#ifdef ESCAPE_FORWARD_SLASH
                case '/': m_sh.append("\\/", 2); break;
#endif
                case '\b': m_sh.append("\\b", 2);
                    break; // \x08
                case '\f': m_sh.append("\\f", 2);
                    break; // \x0C
                case '\n': m_sh.append("\\n", 2);
                    break; // \x0A
                case '\r': m_sh.append("\\r", 2);
                    break; // \x0D
                case '\t': m_sh.append("\\t", 2);
                    break; // \x09
                default:
                    {
                        char buf[7]{};
                        std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned short>(ch));
                        m_sh.append(buf, 6); // \uXXXX一定是6字符
                        break;
                    }
                }
            }
            m_sh.append('\"');
        }

        template <bool Pretty = false>
        void dump(JsonT const& json, std::string_view indent = "\t", int depth = 0)
        {
            std::visit([this, &indent, depth](auto&& v)
                {
                    using T = std::decay_t<decltype(v)>;

                    if constexpr (std::is_same_v<T, std::monostate>)
                        return; // empty basic_json
                    if constexpr (std::is_same_v<T, null_t>)
                    {
                        m_sh.append("null", 4);
                    }
                    if constexpr (std::is_same_v<T, boolean>)
                    {
                        if (v) m_sh.append("true", 4);
                        else m_sh.append("false", 5);
                    }
                    if constexpr (std::is_same_v<T, number_int>)
                    {
                        constexpr size_t MAX_INT_CHARS = 32;
                        char cbuf[MAX_INT_CHARS];
                        auto result = std::to_chars(cbuf, cbuf + sizeof(cbuf), v);
                        m_sh.append(cbuf, result.ptr - cbuf);
                    }
                    if constexpr (std::is_same_v<T, number_float>)
                    {
                        constexpr size_t MAX_DOUBLE_CHARS = 64;
                        char cbuf[MAX_DOUBLE_CHARS];
                        auto result = std::to_chars(cbuf, cbuf + sizeof(cbuf), v, std::chars_format::general);
                        m_sh.append(cbuf, result.ptr - cbuf);

                        std::string_view written(cbuf, result.ptr - cbuf);
                        if (written.find('.') == std::string_view::npos &&
                            written.find('e') == std::string_view::npos &&
                            written.find('E') == std::string_view::npos)
                        {
                            m_sh.append(".0", 2); // ensure that float numbers have a decimal part
                        }
                    }
                    if constexpr (std::is_same_v<T, string>)
                    {
                        escape_string(v);
                    }
                    if constexpr (std::is_same_v<T, array>)
                    {
                        bool first = true;
                        m_sh.append('[');
                        if (!v.empty())
                        {
                            for (auto const& item : v)
                            {
                                if (!first)
                                    m_sh.append(',');
                                first = false;

                                if constexpr (Pretty)
                                    append_indent(indent, depth + 1);
                                dump<Pretty>(item);
                            }
                            if constexpr (Pretty)
                                append_indent(indent, depth);
                        }
                        m_sh.append(']');
                    }
                    if constexpr (std::is_same_v<T, object>)
                    {
                        bool first = true;
                        m_sh.append('{');
                        if (!v.empty())
                        {
                            for (auto const& item : v)
                            {
                                if (!first)
                                    m_sh.append(',');
                                first = false;

                                if constexpr (Pretty)
                                    append_indent(indent, depth + 1);
                                escape_string(item.first);
                                if constexpr (Pretty)
                                    m_sh.append(": ", 2);
                                else
                                    m_sh.append(':');
                                dump<Pretty>(item.second);
                            }
                            if constexpr (Pretty)
                                append_indent(indent, depth);
                        }
                        m_sh.append('}');
                    }
                }, json.m_value
            );
        }
    }; // class JsonSerializer

}

#endif //JSONPP_SERIALIZER_HPP
