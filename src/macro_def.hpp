#ifndef JSONPP_MACRO_DEF_HPP
#define JSONPP_MACRO_DEF_HPP

#define BASIC_JSON_TEMPLATE \
template <  \
template<typename U, typename V, typename... Args> class ObjectType,    \
template<typename U, typename... Args> class ArrayType, \
typename StringType,    \
typename BooleanType,   \
typename NumberIntegerType, \
typename NumberFloatType,   \
template<typename U> class AllocatorType,   \
typename CustomBaseClass    \
>

#define BASIC_JSON_TYPE \
basic_json<ObjectType, ArrayType, StringType, BooleanType, NumberIntegerType, NumberFloatType, AllocatorType, CustomBaseClass>


#endif //JSONPP_MACRO_DEF_HPP