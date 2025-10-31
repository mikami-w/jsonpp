//
// Created on 2025/10/25.
//

#ifndef JSONpp_H
#define JSONpp_H

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <type_traits>

#include "jsonpp.h"

namespace JSONpp{
    class JSONValue;

    using JNull = std::monostate;
    using JArray = std::vector<JSONValue>;
    using JObject = std::unordered_map<std::string, JSONValue>;


#if __cplusplus >= 202002L
    template<typename T>
    concept JsonValueType =
        std::is_same_v<T, JNull> ||
        std::is_same_v<T, std::nullptr_t> ||
        std::is_same_v<T, bool> ||
        std::is_integral_v<T> ||
        std::is_floating_point_v<T> ||
        std::is_convertible_v<T, std::string> ||
        std::is_same_v<T, JArray> ||
        std::is_same_v<T, JObject>;
#else
    template<typename T>
    inline constexpr bool isJsonValueType =
        std::is_same_v<T, JNull> ||
        std::is_same_v<T, std::nullptr_t> ||
        std::is_same_v<T, bool> ||
        std::is_integral_v<T> ||
        std::is_floating_point_v<T> ||
        std::is_convertible_v<T, std::string> ||
        std::is_same_v<T, JArray> ||
        std::is_same_v<T, JObject>;
#endif

    class JSONValue
    {
        std::variant
        <
            JNull,
            bool,
            std::int64_t,
            double,
            std::string,
            JArray,
            JObject
        > value;

    public:
        /*
         * constructors
         */
        explicit JSONValue(bool val): value(val) {};

        JSONValue() = default;
        JSONValue(JNull): value(JNull()) {}
        JSONValue(std::nullptr_t): value(JNull()) {}

#if __cplusplus >= 202002L
        // 对整形的构造函数(含char, 会将char作为整形构造)
        template <typename T_Integer>
            requires std::is_integral_v<T_Integer>
        JSONValue(T_Integer val): value(val) {}
        // 对浮点数的构造函数
        template <typename T_Float>
            requires std::is_floating_point_v<T_Float>
        JSONValue(T_Float val): value(val) {}
#else
        // 对整形的构造函数(含char, 会将char作为整形构造)
        template <typename T_Integer,
            std::enable_if_t<std::is_integral_v<T_Integer>, int> = 0>
        JSONValue(T_Integer val): value(val) {}
        // 对浮点数的构造函数
        template <typename T_Float,
            std::enable_if_t<std::is_floating_point_v<T_Float>, int> = 0>
        JSONValue(T_Float val): value(val) {}

#endif

        JSONValue(char const* val): value(val) {}
        JSONValue(std::string val): value(std::move(val)) {}
        JSONValue(JArray val): value(std::move(val)) {}
        JSONValue(JObject val): value(std::move(val)) {}
        /*
         * end constructors
         */

#if __cplusplus >= 202002L
        // operator =
        template<JsonValueType T>
        JSONValue& operator=(T val) { value = std::move(val); return *this; }
#else
        template<typename T,
            std::enable_if_t<isJsonValueType<T>, int> = 0>
        JSONValue& operator=(T val) { value = std::move(val); return *this; }
#endif

        JSONValue& operator=(std::nullptr_t) { value = JNull(); return *this; }
        JSONValue& operator=(std::initializer_list<JSONValue>);
        // end operator =

        /*
         * type checkers
         */
        bool is_null() const { return std::holds_alternative<JNull>(value); }
        bool is_bool() const { return std::holds_alternative<bool>(value); }
        bool is_number() const { return std::holds_alternative<std::int64_t>(value) || std::holds_alternative<double>(value); }
        bool is_int64() const { return std::holds_alternative<std::int64_t>(value); }
        bool is_double() const { return std::holds_alternative<double>(value); }
        bool is_string() const { return std::holds_alternative<std::string>(value); }
        bool is_array() const { return std::holds_alternative<JArray>(value); }
        bool is_object() const { return std::holds_alternative<JObject>(value); }
        /*
         * end type checkers
         */

        /*
         * safe accessors
         */
        JNull const* get_if_null() const { return std::get_if<JNull>(&value); }
        bool const* get_if_bool() const { return std::get_if<bool>(&value); }
        std::int64_t const* get_if_int64() const { return std::get_if<std::int64_t>(&value); }
        double const* get_if_double() const { return std::get_if<double>(&value); }
        std::string const* get_if_string() const { return std::get_if<std::string>(&value); }
        JArray const* get_if_array() const { return std::get_if<JArray>(&value); }
        JObject const* get_if_object() const { return std::get_if<JObject>(&value); }

        bool* get_if_bool() { return std::get_if<bool>(&value); }
        std::int64_t* get_if_int64() { return std::get_if<std::int64_t>(&value); }
        double* get_if_double() { return std::get_if<double>(&value); }
        std::string* get_if_string() { return std::get_if<std::string>(&value); }
        JArray* get_if_array() { return std::get_if<JArray>(&value); }
        JObject* get_if_object() { return std::get_if<JObject>(&value); }
        /*
         * end safe accessors
         */

        /*
         * asserted accessors
         */
        bool as_bool() const;
        std::int64_t as_int64() const;
        double as_double() const;
        std::string const& as_string() const;
        JArray const& as_array() const;
        JObject const& as_object() const;

        bool& as_bool();
        std::int64_t& as_int64();
        double& as_double();
        std::string& as_string();
        JArray& as_array();
        JObject& as_object();
        /*
         * end asserted accessors
         */

        /*
         * accessors for JArray and JObject
         */
        JSONValue& operator[](size_t index) { return as_array().at(index); }
        const JSONValue& operator[](size_t index) const { return as_array().at(index); }

        JSONValue& operator[](std::string const& key) { return as_object().at(key); }
        const JSONValue& operator[](std::string const& key) const { return as_object().at(key); }
        /*
         * end accessors for JArray and JObject
         */

        // 流自带将需要转义的字符添加 \ (实现其转义)的功能, 该过程发生在从内存到流的过程中
        friend std::ostream& operator<<(std::ostream& os, JSONValue const& val);

        std::string stringify() const;

    }; // class JSONValue

    /*
     * Parse a document (string) to std::optional<JSONValue>.
     * If the document is empty (or contains nothing but whitespace), it will return std::optional(std::nullopt)
     */
    std::optional<JSONValue> parse(std::string_view json_str);

} // namespace JSONpp

#endif //JSONpp_H
