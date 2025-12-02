#ifndef JSONPP_STREAM_TRAITS_HPP
#define JSONPP_STREAM_TRAITS_HPP

#include <type_traits>
#include <cstddef>

namespace JSONpp::traits
{
    namespace details_t
    {
        template <typename T, typename = void>
        struct isJsonStreamImpl: std::false_type {};

        template <typename T>
        struct isJsonStreamImpl<T, std::void_t<
            decltype(std::declval<T>().peek()),
            decltype(std::declval<T>().advance()),
            decltype(std::declval<T>().tell_pos()),
            decltype(std::declval<T>().eof())
        >>
        {
        private:
            using peek_return_t = decltype(std::declval<T>().peek());
            using advance_return_t = decltype(std::declval<T>().advance());
            using tell_pos_return_t = decltype(std::declval<T>().tell_pos());
            using eof_return_t = decltype(std::declval<T>().eof());

        public:
            static constexpr bool value = std::conjunction_v<
                std::is_same<peek_return_t, int>,
                std::is_same<advance_return_t, int>,
                std::is_same<tell_pos_return_t, std::size_t>,
                std::is_same<eof_return_t, bool>
            >;
        };

        struct PredicateFunctor
        {
            bool operator()(char const&) const { return false; }
        };

        // trait for std::map
        template <typename T, typename = void>
        struct has_key_compare: std::false_type {};

        template <typename T>
        struct has_key_compare<T, std::void_t<typename T::key_compare>>: std::true_type {};

        // trait for std::unordered_map
        template <typename T, typename = void>
        struct has_hasher: std::false_type {};

        template <typename T>
        struct has_hasher<T, std::void_t<typename T::hasher>>: std::true_type {};
    }

    template <typename T>
    struct isJsonStream: details_t::isJsonStreamImpl<T> {};

    template <typename T>
    inline constexpr bool isJsonStream_v = isJsonStream<T>::value;

    // Is the stream capable of directly obtaining its complete size
    template <typename T, typename = void>
    struct isSizedStream: std::false_type {};

    template <typename T>
    struct isSizedStream<T, std::void_t<decltype(std::declval<T>().size())>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool isSizedStream_v = isSizedStream<T>::value;

    // Is the stream a seekable stream (provides seek capability)
    template <typename T, typename = void>
    struct isSeekableStream: std::false_type {};

    template <typename T>
    struct isSeekableStream<T,
        std::enable_if_t<isSizedStream_v<T>,
        std::void_t<
            decltype(std::declval<T>().seek(std::size_t()))
    >>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool isSeekableStream_v = isSeekableStream<T>::value;

    // Is the stream a contiguous stream (provides chunk access capability)
    template <typename T, typename = void>
    struct isContiguousStream : std::false_type {};

    template <typename T>
    struct isContiguousStream<T,
        std::enable_if_t<isSeekableStream_v<T>,
        std::void_t<
            decltype(std::declval<T>().get_chunk(std::size_t(), std::size_t())),
            decltype(std::declval<T&>().read_chunk_until(std::declval<details_t::PredicateFunctor>()))
    >>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool isContiguousStream_v = isContiguousStream<T>::value;

    // trait for JSON Serialize Handler
    template <typename T, typename = void>
    struct isJsonSerializeHandler : std::false_type {};

    template <typename T>
    struct isJsonSerializeHandler<T, std::void_t<
        decltype(std::declval<T>().append(char())),
        decltype(std::declval<T>().append(std::string_view())),
        decltype(std::declval<T>().append((char const*)0, std::size_t()))
    >>
        : std::true_type {};

    template <typename T>
    inline constexpr bool isJsonSerializeHandler_v = isJsonSerializeHandler<T>::value;
}

#endif //JSONPP_STREAM_TRAITS_HPP