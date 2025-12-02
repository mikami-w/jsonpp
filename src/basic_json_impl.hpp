#ifndef JSONPP_BASIC_JSON_IMPL_HPP
#define JSONPP_BASIC_JSON_IMPL_HPP

#include "macro_def.hpp"
#include "basic_json.hpp"

namespace JSONpp
{
    using namespace traits;

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::dump(std::string& buffer) const
    {
        std::visit(
            [&buffer](auto&& v)
            {
                using T = std::decay_t<decltype(v)>;

                if constexpr (std::is_same_v<T, std::monostate>)
                    return; // empty basic_json
                if constexpr (std::is_same_v<T, null_t>)
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
                    char cbuf[MAX_DOUBLE_CHARS];
                    auto result = std::to_chars(cbuf, cbuf + sizeof(cbuf), v, std::chars_format::general);
                    buffer.append(cbuf, result.ptr - cbuf);
                }
                if constexpr (std::is_same_v<T, std::string>)
                {
                    details::escape_string(buffer, v);
                }
                if constexpr (std::is_same_v<T, array>)
                {
                    bool first = true;
                    buffer.push_back('[');
                    for (auto const& item : v)
                    {
                        if (first)
                        {
                            first = false;
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
}


#endif //JSONPP_BASIC_JSON_IMPL_HPP