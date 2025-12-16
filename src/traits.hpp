#ifndef JSONPP_STREAM_TRAITS_HPP
#define JSONPP_STREAM_TRAITS_HPP

#include <type_traits>
#include <cstddef>
#include <string_view>

namespace JSONpp::traits
{
    namespace details_t
    {
        template <typename T, typename = void>
        struct is_json_stream_impl: std::false_type {};

        template <typename T>
        struct is_json_stream_impl<T, std::void_t<
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

        template <typename T>
        inline constexpr bool has_key_compare_v = has_key_compare<T>::value;

        // trait for std::unordered_map
        template <typename T, typename = void>
        struct has_hasher: std::false_type {};

        template <typename T>
        struct has_hasher<T, std::void_t<typename T::hasher>>: std::true_type {};

        template <typename T>
        inline constexpr bool has_hasher_v = has_hasher<T>::value;
    }

    template <typename T>
    struct is_json_stream: details_t::is_json_stream_impl<T> {};

    template <typename T>
    inline constexpr bool is_json_stream_v = is_json_stream<T>::value;

    // Is the stream capable of directly obtaining its complete size
    template <typename T, typename = void>
    struct is_sized_stream: std::false_type {};

    template <typename T>
    struct is_sized_stream<T, std::void_t<decltype(std::declval<T>().size())>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool is_sized_stream_v = is_sized_stream<T>::value;

    // Is the stream a seekable stream (provides seek capability)
    template <typename T, typename = void>
    struct is_seekable_stream: std::false_type {};

    template <typename T>
    struct is_seekable_stream<T,
        std::enable_if_t<is_sized_stream_v<T>,
        std::void_t<
            decltype(std::declval<T>().seek(std::size_t()))
    >>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool is_seekable_stream_v = is_seekable_stream<T>::value;

    // Is the stream a contiguous stream (provides chunk access capability)
    template <typename T, typename = void>
    struct is_contiguous_stream : std::false_type {};

    template <typename T>
    struct is_contiguous_stream<T,
        std::enable_if_t<is_seekable_stream_v<T>,
        std::void_t<
            decltype(std::declval<T>().get_chunk(std::size_t(), std::size_t())),
            decltype(std::declval<T&>().read_chunk_until(std::declval<details_t::PredicateFunctor>()))
    >>>
        : std::true_type {};

    template <typename T>
    inline constexpr bool is_contiguous_stream_v = is_contiguous_stream<T>::value;

    // trait for JSON Serialize Handler
    template <typename T, typename = void>
    struct is_json_serialize_handler : std::false_type {};

    template <typename T>
    struct is_json_serialize_handler<T, std::void_t<
        decltype(std::declval<T>().append(char())),
        decltype(std::declval<T>().append(std::string_view())),
        decltype(std::declval<T>().append((char const*)0, std::size_t()))
    >>
        : std::true_type {};

    template <typename T>
    inline constexpr bool is_json_serialize_handler_v = is_json_serialize_handler<T>::value;
}

#endif //JSONPP_STREAM_TRAITS_HPP