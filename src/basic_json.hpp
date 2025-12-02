#ifndef JSONPP_HPP
#define JSONPP_HPP

#include "json_fwd.hpp"
#include "serializer.hpp"
#include "jsonexception.hpp"
#include "json_stream_adaptor.hpp"
#include "macro_def.hpp"

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace JSONpp
{
    namespace details
    {
        struct EmptyBaseClass {};
    }

    using null_t = std::nullptr_t;
    constexpr null_t null = nullptr;

    BASIC_JSON_TEMPLATE
    class basic_json : public std::conditional_t<std::is_same_v<CustomBaseClass, void>, details::EmptyBaseClass, CustomBaseClass>
    {
    public:
        using boolean = BooleanType;
        using number_int = NumberIntegerType;
        using number_float = NumberFloatType;
        using string = StringType;
        using array = ArrayType<basic_json, AllocatorType<basic_json>>;
        using object = ObjectType<StringType, basic_json, std::less<StringType>, AllocatorType<std::pair<const StringType, basic_json>>>;

        using JsonType = std::variant <
            std::monostate,
            null_t,
            boolean,
            number_int,
            number_float,
            string,
            array,
            object
        >;

        using json_type = BASIC_JSON_TYPE;

        template <typename T>
        constexpr static bool isJsonValueType =
            std::is_same_v<T, std::monostate> ||
            std::is_same_v<T, null_t> ||
            std::is_same_v<T, boolean> ||
            std::is_integral_v<T> ||
            std::is_same_v<T, number_int> ||
            std::is_floating_point_v<T> ||
            std::is_same_v<T, number_float> ||
            std::is_convertible_v<T, string> ||
            std::is_same_v<T, array> ||
            std::is_same_v<T, object>;

    private:
        JsonType value;

        template <typename T>
        static T& as_impl(JsonType& v, char const* typeName)
        {
            try
            {
                return std::get<T>(v);
            }
            catch (const std::bad_variant_access&)
            {
                throw JsonTypeError(std::string("Value is not a ") + typeName);
            }
        }

        template <typename T>
        static T const& as_impl(JsonType const& v, char const* typeName)
        {
            try
            {
                return std::get<T>(v);
            }
            catch (const std::bad_variant_access&)
            {
                throw JsonTypeError(std::string("Value is not a ") + typeName);
            }
        }

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
        /*
         * end constructors
         */

        template <typename T,
            std::enable_if_t<isJsonValueType<T>, int> = 0>
        basic_json& operator=(T val) { value = std::move(val); return *this; }

        basic_json& operator=(std::initializer_list<basic_json>); // TODO: implement
        // end operator =

        /*
         * type checkers
         */
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

        basic_json& operator[](std::string const& key) { return as_object()[key]; }
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
        // void dump(std::ostream& os) const;

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
        std::string buffer;
        val.dump(buffer);
        return os << buffer;
    }


} // namespace JSONpp

#endif //JSONPP_HPP
