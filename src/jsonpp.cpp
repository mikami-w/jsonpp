#include "jsonpp.hpp"

#include <filesystem>
#include <charconv>
#include "stream_traits.hpp"
#include "json_stream_adaptor.hpp"
#include "serializer.hpp"

namespace JSONpp
{
    using namespace traits;

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
