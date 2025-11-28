//
// Created on 2025/10/31.
//

#ifndef JSONPP_JSON_STREAM_ADAPTOR_HPP
#define JSONPP_JSON_STREAM_ADAPTOR_HPP

#include <cstddef>
#include <algorithm>
#include <cassert>
#include <type_traits>
#include <string_view>

namespace JSONpp
{
    // 流适配器
    class StringViewStream
    {
        std::string_view data;
        size_t pos;

    public:
        char peek() const noexcept { if (pos < data.size()) return data[pos]; else return 0; }
        char advance() noexcept { if (pos < data.size()) return data[pos++]; else return 0; }
        size_t tell_pos() const noexcept { return pos; }
        size_t size() const noexcept { return data.size(); }
        bool eof() { return pos >= data.size(); }

        template <typename FunctorT>
        std::string_view read_chunk_until(FunctorT predicate) &;

        explicit StringViewStream(std::string_view doc): data(doc), pos(0) {}
    };

    template <typename FunctorT>
    std::string_view StringViewStream::read_chunk_until(FunctorT predicate) &
    {
        std::string_view remaining = data.substr(pos);
        auto it = std::find_if(remaining.begin(), remaining.end(), predicate);
        auto chunk_size = std::distance(remaining.begin(), it);
        // 获取块 (如果 chunk_size=0 返回空视图)
        std::string_view chunk = data.substr(pos, chunk_size);

        pos += chunk_size;

        return chunk;
    }

    class IstreamStream
    {

    };

}
#endif //JSONPP_JSON_STREAM_ADAPTOR_HPP