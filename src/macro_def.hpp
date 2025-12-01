#ifndef JSONPP_MACRO_DEF_HPP
#define JSONPP_MACRO_DEF_HPP

#define MAX_NESTING_DEPTH 1024  // Change this value as needed

#define JSONPP_IMPORT_PARSERBASE_MEMBERS_ \
using ParserBase<StreamT>::m_stream;        \
using ParserBase<StreamT>::peek;            \
using ParserBase<StreamT>::advance;         \
using ParserBase<StreamT>::tell_pos;        \
using ParserBase<StreamT>::eof;             \
using ParserBase<StreamT>::consume;         \
using ParserBase<StreamT>::size;            \
using ParserBase<StreamT>::seek;            \
using ParserBase<StreamT>::get_chunk;       \
using ParserBase<StreamT>::read_chunk_until;

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

#define _STR(x) #x
#define TO_STRING(x) _STR(x)

#define LOG_VAR(x) std::cout << "Variable " << #x << " = " << x << std::endl;

#define JSONPP_CHECK_EOF_(type, start) \
do { \
    if (eof()) \
    throw JsonParseError(("Unexpected end of file while parsing " type ", starting"), start); \
} while(0)

#endif //JSONPP_MACRO_DEF_HPP
