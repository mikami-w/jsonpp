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
    class JSONValue;

    using JNull = std::monostate;
    using JArray = std::vector<JSONValue>;
    using JObject = std::unordered_map<std::string, JSONValue>;

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
        JSONValue(std::nullptr_t): value(JNull()) {}
        // 对整形的构造函数(含char, 会将char作为整形构造)
        template <typename T_Integer>
            requires std::is_integral_v<T_Integer>
        JSONValue(T_Integer val): value(val) {}
        // 对浮点数的构造函数
        template <typename T_Float>
            requires std::is_floating_point_v<T_Float>
        JSONValue(T_Float val): value(val) {}

        JSONValue(char const* val): value(val) {}
        JSONValue(std::string val): value(std::move(val)) {}
        JSONValue(JArray val): value(std::move(val)) {}
        JSONValue(JObject val): value(std::move(val)) {}
        /*
         * end constructors
         */

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
        std::int64_t* get_if_int() { return std::get_if<std::int64_t>(&value); }
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
        JSONValue& operator[](size_t index);
        const JSONValue& operator[](size_t index) const;

        JSONValue& operator[](std::string const& key);
        const JSONValue& operator[](std::string const& key) const;
        /*
         * end accessors for JArray and JObject
         */


    };

    JSONValue parse(std::string_view json_str);

} // namespace JSONpp

#endif //JSONpp_H
