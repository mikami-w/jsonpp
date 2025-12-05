#ifndef JSONPP_BASIC_JSON_HPP
#define JSONPP_BASIC_JSON_HPP

#include "json_fwd.hpp"
#include "json_serializer.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"
#include "macro_def.hpp"
#include "traits.hpp"

#include <string>
#include <variant>
#include <type_traits>
#include <cstdint>

namespace JSONpp
{
    namespace details
    {
        struct EmptyBaseClass {};
    }

    enum class Type: std::uint8_t
    {
        empty,
        null,
        boolean,
        number_int,
        number_float,
        string,
        array,
        object
    };

    BASIC_JSON_TEMPLATE
    class basic_json : public std::conditional_t<std::is_same_v<CustomBaseClass, void>, details::EmptyBaseClass, CustomBaseClass>
    {
    private:
        using _test_object_type = ObjectType<StringType, int>;
        static constexpr bool _is_map_like =
            traits::details_t::has_key_compare<_test_object_type>::value;
        static constexpr bool _is_unordered_map_like =
            traits::details_t::has_hasher<_test_object_type>::value;
        using _object_allocator_t = AllocatorType<std::pair<const StringType, basic_json>>;

        template <bool IsMapLike, bool IsUnorderedMapLike, typename Dummy = void>
        struct _object_type_selector
        {
            // assume it only needs K, V, Allocator (non-standard container)
            using type = ObjectType<StringType,
                                   basic_json,
                                   _object_allocator_t>;
        };

        template <typename Dummy>
        struct _object_type_selector<true, false, Dummy>
        {
            // assume it's a std::map-like container
            using type = ObjectType<StringType,
                                    basic_json,
                                    std::less<StringType>,
                                    _object_allocator_t>;
        };

        template <typename Dummy>
        struct _object_type_selector<false, true, Dummy>
        {
            // assume it's a std::unordered_map-like container
            using type = ObjectType<StringType,
                                    basic_json,
                                    std::hash<StringType>,
                                    std::equal_to<StringType>,
                                    _object_allocator_t>;
        };

    public:
        using boolean = BooleanType;
        using number_int = NumberIntegerType;
        using number_float = NumberFloatType;
        using string = StringType;
        using array = ArrayType<basic_json, AllocatorType<basic_json>>;
        using object = typename _object_type_selector<_is_map_like, _is_unordered_map_like>::type;

        using value_t = std::variant <
            std::monostate,
            null_t,
            boolean,
            number_int,
            number_float,
            string,
            array,
            object
        >;

        using json_t = BASIC_JSON_TYPE;

        template <typename T>
        constexpr static bool isJsonValueType =
            std::is_same_v<T, std::monostate> ||
            std::is_same_v<T, null_t> ||
            std::is_same_v<T, bool> ||
            std::is_same_v<T, boolean> ||
            std::is_integral_v<T> ||
            std::is_same_v<T, number_int> ||
            std::is_floating_point_v<T> ||
            std::is_same_v<T, number_float> ||
            std::is_convertible_v<T, string> ||
            std::is_same_v<T, array> ||
            std::is_same_v<T, object>;

    private:
        template <typename JsonT, typename SerializeHandlerT>
        friend class details::JsonSerializer;

    private:
        value_t value;

    private:
        template <Type T>
        void set_type_impl();

        template <typename T>
        static T& as_impl(value_t& v, char const* typeName);

        template <typename T>
        static T const& as_impl(value_t const& v, char const* typeName);

    public:
        /*
         * constructors
         */
        explicit basic_json(boolean val): value(val) {}

        basic_json() = default;
        basic_json(null_t): value(null_t()) {}

        // Constructor for integral types (including char, which will be treated as an integer)
        template <typename T_Integer,
            std::enable_if_t<std::is_integral_v<T_Integer>, int> = 0>
        basic_json(T_Integer val): value(val) {}
        basic_json(number_int val): value(val) {}
        // Constructor for floating-point types
        template <typename T_Float,
            std::enable_if_t<std::is_floating_point_v<T_Float>, int> = 0>
        basic_json(T_Float val): value(val) {}
        basic_json(number_float val): value(val) {}

        basic_json(string val): value(std::move(val)) {}
        basic_json(char const* val): value(val) {}
        explicit basic_json(std::string_view val): value(string(val)) {} // Explicit to prevent expensive, implicit copies from a non-owning string_view.
        basic_json(array val): value(std::move(val)) {}
        basic_json(object val): value(std::move(val)) {}

        basic_json(basic_json const& other) = default;
        basic_json(basic_json&& other) noexcept = default;

        ~basic_json() = default;
        /*
         * end constructors and destructor
         */

        template <Type T>
        void set_type(bool clear_content = false);
        void set_type(Type const& t, bool clear_content = false);

        basic_json& operator=(basic_json const& other) = default;
        basic_json& operator=(basic_json&& other) noexcept = default;

        template <typename T,
            std::enable_if_t<isJsonValueType<std::decay<T>> &&
                            !std::is_same_v<std::decay<T>, basic_json>, int> = 0>
        basic_json& operator=(T&& val) { value = std::forward<T>(val); return *this; }

        void swap(basic_json& other) noexcept { value.swap(other); }
        friend void swap(basic_json& lhs, basic_json& rhs) noexcept { lhs.value.swap(rhs.value); } // for ADL (Argument-Dependent Lookup)

        /*
         * type checkers
         */
        Type type() const noexcept { return static_cast<Type>(value.index()); }
        bool empty() const noexcept { return std::holds_alternative<std::monostate>(value); }
        bool is_null() const noexcept { return std::holds_alternative<null_t>(value); }
        bool is_bool() const noexcept { return std::holds_alternative<boolean>(value); }
        bool is_number() const noexcept { return std::holds_alternative<number_int>(value) || std::holds_alternative<number_float>(value); }
        bool is_int() const noexcept { return std::holds_alternative<number_int>(value); }
        bool is_float() const noexcept { return std::holds_alternative<number_float>(value); }
        bool is_string() const noexcept { return std::holds_alternative<string>(value); }
        bool is_array() const noexcept { return std::holds_alternative<array>(value); }
        bool is_object() const noexcept { return std::holds_alternative<object>(value); }
        /*
         * end type checkers
         */

        /*
         * safe accessors
         */
        boolean const* get_if_bool() const noexcept { return std::get_if<boolean>(&value); }
        number_int const* get_if_int() const noexcept { return std::get_if<number_int>(&value); }
        number_float const* get_if_float() const noexcept { return std::get_if<number_float>(&value); }
        std::string const* get_if_string() const noexcept { return std::get_if<string>(&value); }
        array const* get_if_array() const noexcept { return std::get_if<array>(&value); }
        object const* get_if_object() const noexcept { return std::get_if<object>(&value); }

        boolean* get_if_bool() noexcept { return std::get_if<boolean>(&value); }
        number_int* get_if_int() noexcept { return std::get_if<number_int>(&value); }
        number_float* get_if_float() noexcept { return std::get_if<number_float>(&value); }
        string* get_if_string() noexcept { return std::get_if<string>(&value); }
        array* get_if_array() noexcept { return std::get_if<array>(&value); }
        object* get_if_object() noexcept { return std::get_if<object>(&value); }
        /*
         * end safe accessors
         */

        /*
         * asserted accessors
         */
        boolean as_bool() const { return as_impl<boolean>(value, "bool"); }
        number_int as_int() const { return as_impl<number_int>(value, "int64"); }
        number_float as_float() const { return as_impl<number_float>(value, "double"); }
        string const& as_string() const { return as_impl<string>(value, "string"); }
        array const& as_array() const { return as_impl<array>(value, "array"); }
        object const& as_object() const { return as_impl<object>(value, "object"); }

        boolean& as_bool() { return as_impl<boolean>(value, "bool"); }
        number_int& as_int() { return as_impl<number_int>(value, "int64"); }
        number_float& as_float() { return as_impl<number_float>(value, "double"); }
        std::string& as_string() { return as_impl<string>(value, "string"); }
        array& as_array() { return as_impl<array>(value, "array"); }
        object& as_object() { return as_impl<object>(value, "object"); }
        /*
         * end asserted accessors
         */

        /*
         * accessors for array and object
         */
        basic_json& operator[](std::size_t index);
        basic_json const& operator[](std::size_t index) const;
        basic_json& at(std::size_t);
        basic_json const& at(std::size_t) const;

        basic_json& operator[](std::string const& key);
        basic_json const& operator[](std::string const& key) const;
        basic_json& at(std::string const& key);
        basic_json const& at(std::string const& key) const;
        /*
         * end accessors for array and object
         */

        bool operator==(basic_json const& other) const;
        bool operator!=(basic_json const& other) const { return !(*this == other); }

        static basic_json parse(std::string_view json_doc);
        static basic_json parse(std::istream& json_istream);
        template <typename StreamT,
            std::enable_if_t<traits::isJsonStream_v<StreamT>, int> = 0>
        static basic_json parse(StreamT& stream);

        void dump(std::string& buffer) const;
        void dump(std::ostream& os) const;
        template <typename SerializeHandlerT,
            std::enable_if_t<traits::isJsonSerializeHandler_v<SerializeHandlerT>, int> = 0>
        void dump(SerializeHandlerT& handler);

        // friend std::ostream& operator<<(std::ostream& os, basic_json const& val);

        std::string stringify() const
        {
            std::string buffer;
            dump(buffer);
            return buffer;
        }

    }; // class basic_json

    // The stream itself has the function of adding \ (escaping) to characters that need to be escaped. This process occurs from memory to the stream.
    BASIC_JSON_TEMPLATE
    std::ostream& operator<<(std::ostream& os, BASIC_JSON_TYPE const& val)
    {
        val.dump(os);
        return os;
    }


} // namespace JSONpp

#endif //JSONPP_BASIC_JSON_HPP
