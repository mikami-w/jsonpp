//
// Created on 2025/10/25.
//

#ifndef JSONpp_H
#define JSONpp_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include "jsonpp.h"

namespace JSONpp{
    class json;

    using null = std::nullptr_t;
    using array = std::vector<json>;
    using object = std::unordered_map<std::string, json>;

    template <typename T>
    inline constexpr bool isJsonValueType =
        std::is_same_v<T, std::monostate> ||
        std::is_same_v<T, null> ||
        std::is_same_v<T, bool> ||
        std::is_integral_v<T> ||
        std::is_floating_point_v<T> ||
        std::is_convertible_v<T, std::string> ||
        std::is_same_v<T, array> ||
        std::is_same_v<T, object>;

    class json
    {
        std::variant
        <
            std::monostate, // 用于表示空的json
            null,
            bool,
            std::int64_t,
            double,
            std::string,
            array,
            object
        > value;

    public:
        /*
         * constructors
         */
        explicit json(bool val): value(val) {}

        json() = default;
        json(null): value(null()) {}

        // Constructor for integral types (including char, which will be treated as an integer)
        template <typename T_Integer,
            std::enable_if_t<std::is_integral_v<T_Integer>, int> = 0>
        json(T_Integer val): value(val) {}
        // Constructor for floating-point types
        template <typename T_Float,
            std::enable_if_t<std::is_floating_point_v<T_Float>, int> = 0>
        json(T_Float val): value(val) {}

        json(char const* val): value(val) {}
        json(std::string val): value(std::move(val)) {}
        explicit json(std::string_view val): value(std::string(val)) {} // Explicit to prevent expensive, implicit copies from a non-owning string_view.
        json(array val): value(std::move(val)) {}
        json(object val): value(std::move(val)) {}
        /*
         * end constructors
         */

        template <typename T,
            std::enable_if_t<isJsonValueType<T>, int> = 0>
        json& operator=(T val) { value = std::move(val); return *this; }

        json& operator=(std::initializer_list<json>); // TODO: implement
        // end operator =

        /*
         * type checkers
         */
        bool empty() const { return std::holds_alternative<std::monostate>(value); }
        bool is_null() const { return std::holds_alternative<null>(value); }
        bool is_bool() const { return std::holds_alternative<bool>(value); }
        bool is_number() const { return std::holds_alternative<std::int64_t>(value) || std::holds_alternative<double>(value); }
        bool is_int() const { return std::holds_alternative<std::int64_t>(value); }
        bool is_float() const { return std::holds_alternative<double>(value); }
        bool is_string() const { return std::holds_alternative<std::string>(value); }
        bool is_array() const { return std::holds_alternative<array>(value); }
        bool is_object() const { return std::holds_alternative<object>(value); }
        /*
         * end type checkers
         */

        /*
         * safe accessors
         */
        bool const* get_if_bool() const { return std::get_if<bool>(&value); }
        std::int64_t const* get_if_int() const { return std::get_if<std::int64_t>(&value); }
        double const* get_if_float() const { return std::get_if<double>(&value); }
        std::string const* get_if_string() const { return std::get_if<std::string>(&value); }
        array const* get_if_array() const { return std::get_if<array>(&value); }
        object const* get_if_object() const { return std::get_if<object>(&value); }

        bool* get_if_bool() { return std::get_if<bool>(&value); }
        std::int64_t* get_if_int() { return std::get_if<std::int64_t>(&value); }
        double* get_if_float() { return std::get_if<double>(&value); }
        std::string* get_if_string() { return std::get_if<std::string>(&value); }
        array* get_if_array() { return std::get_if<array>(&value); }
        object* get_if_object() { return std::get_if<object>(&value); }
        /*
         * end safe accessors
         */

        /*
         * asserted accessors
         */
        bool as_bool() const;
        std::int64_t as_int() const;
        double as_float() const;
        std::string const& as_string() const;
        array const& as_array() const;
        object const& as_object() const;

        bool& as_bool();
        std::int64_t& as_int();
        double& as_float();
        std::string& as_string();
        array& as_array();
        object& as_object();
        /*
         * end asserted accessors
         */

        /*
         * accessors for array and object
         */
        json& operator[](size_t index) { return as_array().at(index); }
        const json& operator[](size_t index) const { return as_array().at(index); }

        json& operator[](std::string const& key) { return as_object().at(key); }
        const json& operator[](std::string const& key) const { return as_object().at(key); }
        /*
         * end accessors for array and object
         */

        // The stream itself has the function of adding \ (escaping) to characters that need to be escaped. This process occurs from memory to the stream.
        friend std::ostream& operator<<(std::ostream& os, json const& val);

        std::string stringify() const;

    }; // class json

    /*
     * Parse a document (string) to json.
     * If the document is empty (or contains nothing but whitespace), the returned value's empty() will be true, otherwise empty() will be false.
     */
    json parse(std::string_view json_str);

} // namespace JSONpp

#endif //JSONpp_H
