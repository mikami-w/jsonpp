//
// Created on 2025/10/31.
//

#ifndef JSONPP_BASIC_JSON_STREAM_H
#define JSONPP_BASIC_JSON_STREAM_H

#include <cstddef>
#include <type_traits>

namespace JSONpp
{
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
    struct isSeekableStream<T, std::void_t<
        decltype(std::declval<T>().seek(size_t())),
        decltype(std::declval<T>().get_chunk(size_t(), size_t()))
    >>
        : std::true_type {};

    template<typename T>
    inline constexpr bool isSeekableStream_v = isSeekableStream<T>::value;

    // 定义解析器需要的流
#if __cplusplus >= 202002L
    template<typename T>
    concept JsonStream = requires(T stream)
    {
        { stream.peek() } -> std::same_as<char>;  // 必须能 "偷看"
        { stream.advance() } -> std::same_as<char>; // 必须能 "前进"
        { stream.tell_pos() } -> std::same_as<size_t>; // 必须能报告位置
    };
#else
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
    }

    template<typename T>
    struct isJsonStream: _details::isJsonStreamImpl<T> {};

    template<typename T>
    inline constexpr bool isJsonStream_v = isJsonStream<T>::value;
#endif
}
#endif //JSONPP_BASIC_JSON_STREAM_H