#ifndef JSONPP_JSON_FWD_HPP
#define JSONPP_JSON_FWD_HPP

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <memory> // for std::allocator

namespace JSONpp
{
    using null_t = std::nullptr_t;
    constexpr null_t null = nullptr;

    template <
        template<typename U, typename V, typename... Args> class ObjectType = std::map,
        template<typename U, typename... Args> class ArrayType = std::vector,
        typename StringType = std::string,
        typename BooleanType = bool,
        typename NumberIntegerType = std::int64_t,
        typename NumberFloatType = double,
        template<typename U> class AllocatorType = std::allocator,
        // template<typename T, typename SFINAE = void> class JSONSerializer = adl_serializer, // 待实现
        // typename BinaryType = std::vector<std::uint8_t>, // 待实现
        typename CustomBaseClass = void
    >
    class basic_json;

    using json = basic_json<>;
    // using unordered_json = basic_json<std::unordered_map>; // 目前不支持, 需完善ObjectType的SFINAE模板识别

}
#endif //JSONPP_JSON_FWD_HPP