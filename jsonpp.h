//
// Created on 2025/10/25.
//

#ifndef JSONpp_H
#define JSONpp_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <unordered_map>

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
        JSONValue() = default;
    };


} // namespace JSONpp

#endif //JSONpp_H
