#ifndef JSONPP_SERIALIZER_HPP
#define JSONPP_SERIALIZER_HPP

#include <algorithm>
#include <array>
#include <iostream>

#include "jsonpp.hpp"

namespace JSONpp
{
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

    inline void escape_string(std::string& buffer, std::string_view str) // escape v.转义 e.g. \ -> \\, " -> \"
    {
        buffer.push_back('\"');
        auto chunkBegin = str.begin();
        auto const end = str.end();
        while (chunkBegin < end)
        {
            auto badChar = std::find_if(chunkBegin, end, needs_escaping);
            auto chunkLength = badChar - chunkBegin;
            if (chunkLength > 0)
            {
                buffer.append(std::addressof(*chunkBegin), chunkLength); // 第一个参数应当为指针而不是迭代器, 尽管部分实现中迭代器底层直接使用指针
            }
            chunkBegin = badChar + 1; // 跳过当前已经写入流的块, 等同于 chunkBegin += chunkLength + 1

            // 下面处理转义
            if (badChar == end)
                break;

            char ch = *badChar;
            switch (ch)
            {
            case '\"': buffer.append("\\\"", 2);
                break;
            case '\\': buffer.append("\\\\", 2);
                break;
#ifdef ESCAPE_FORWARD_SLASH
            case '/': buffer.append("\\/", 2); break;
#endif
            case '\b': buffer.append("\\b", 2);
                break; // \x08
            case '\f': buffer.append("\\f", 2);
                break; // \x0C
            case '\n': buffer.append("\\n", 2);
                break; // \x0A
            case '\r': buffer.append("\\r", 2);
                break; // \x0D
            case '\t': buffer.append("\\t", 2);
                break; // \x09
            default:
                {
                    char buf[7]{};
                    std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned short>(ch));
                    buffer.append(buf, 6); // \uXXXX一定是6字符
                    break;
                }
            }
        }

        // return buffer << '\"';
        buffer.push_back('\"');
    }

    /*
     * end stringifier
     */
}

#endif //JSONPP_SERIALIZER_HPP
