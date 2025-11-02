//
// Created on 2025/10/31.
//

#ifndef JSONPP_BASIC_JSON_STREAM_H
#define JSONPP_BASIC_JSON_STREAM_H

#include <cstddef>
#include <type_traits>
#include <string_view>

namespace JSONpp
{
    namespace _details
    {
        // 默认无效
        template<typename T, typename = void>
        struct isJsonStreamImpl: std::false_type {};

        // 有效偏特化
        template<typename T>
        struct isJsonStreamImpl<T, std::void_t<
                                    decltype(std::declval<T>().peek()),
                                    decltype(std::declval<T>().advance()),
                                    decltype(std::declval<T>().tell_pos())
                                >>
        {
        private:
            using peek_return_t = decltype(std::declval<T>().peek());
            using advance_return_t = decltype(std::declval<T>().advance());
            using get_pos_return_t = decltype(std::declval<T>().tell_pos());

        public:
            static constexpr bool value = std::conjunction_v<
                std::is_same<peek_return_t, char>,
                std::is_same<advance_return_t, char>,
                std::is_same<get_pos_return_t, size_t>
            >;
        };

        struct PredicateFunctor
        {
            bool operator()(const char& c) const { return false; }
        };
    }

    template<typename T>
    struct isJsonStream: _details::isJsonStreamImpl<T> {};

    template<typename T>
    inline constexpr bool isJsonStream_v = isJsonStream<T>::value;

    // 用于区分流能否直接获取完整大小
    template<typename T, typename = void>
    struct isSizedStream: std::false_type {};

    template<typename T>
    struct isSizedStream<T, std::void_t<decltype(std::declval<T>().size())>>
        : std::true_type {};

    template<typename T>
    inline constexpr bool isSizedStream_v = isSizedStream<T>::value;

    // 是否是随机访问流
    template<typename T, typename = void>
    struct isSeekableStream: std::false_type {};

    template<typename T>
    struct isSeekableStream<T, std::enable_if_t<isSizedStream_v<T>,
        std::void_t<
            decltype(std::declval<T>().seek(size_t()))
    >>>
        : std::true_type {};

    template<typename T>
    inline constexpr bool isSeekableStream_v = isSeekableStream<T>::value;

    // 是否连续存储 (提供chunk访问能力)
    template<typename T, typename = void>
    struct isContiguousStream: std::false_type {};

    template<typename T>
    struct isContiguousStream<T,
        std::enable_if_t<isSeekableStream_v<T>,
        std::void_t<
            decltype(std::declval<T>().get_chunk(size_t(), size_t())),
            decltype(std::declval<T>().get_chunk_until(std::declval<_details::PredicateFunctor>()))
    >>>
        : std::true_type {};

    template<typename T>
    inline constexpr bool isContiguousStream_v = isContiguousStream<T>::value;

//     // 定义解析器需要的流
// #if __cplusplus >= 202002L
//     template<typename T>
//     concept JsonStream = requires(T stream)
//     {
//         { stream.peek() } -> std::same_as<char>;  // 必须能 "偷看"
//         { stream.advance() } -> std::same_as<char>; // 必须能 "前进"
//         { stream.tell_pos() } -> std::same_as<size_t>; // 必须能报告位置
//     };
// #else



    // 流适配器
    class StringViewStream
    {
        std::string_view data;
        size_t pos;

    public:
        char peek() const { return data[pos]; }
        char advance() { return data[pos++]; }
        size_t tell_pos() const { return pos; }
        size_t size() const { return data.size(); }
        void seek(size_t step) { pos += step; }
        std::string_view get_chunk(size_t begin, size_t length) { return data.substr(begin, length); }
        bool eof() { return pos >= data.size(); }

        template<typename FunctorT>
        std::string_view get_chunk_until(FunctorT predicate) &;

        explicit StringViewStream(std::string_view doc): data(doc), pos(doc.size()) {}
    };

    template<typename FunctorT>
    std::string_view StringViewStream::get_chunk_until(FunctorT predicate) &
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
#endif //JSONPP_BASIC_JSON_STREAM_H