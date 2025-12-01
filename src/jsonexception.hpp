#ifndef JSONPP_JSONEXCEPTION_HPP
#define JSONPP_JSONEXCEPTION_HPP
#include <stdexcept>
#include "macro_def.hpp"

namespace JSONpp
{
    /*
     * JSON exceptions
     */
    class JsonException : public std::runtime_error
    {
    public:
        JsonException(std::string const& msg):
            std::runtime_error(msg) {}
    };

    class JsonParseError : public JsonException
    {
    public:
        static constexpr char const* UNPARSABLE_MESSAGE = "Unparsable character(s)";
        static constexpr char const* UNEXPECTED_EOF_MESSAGE = "Unexpected end of file while parsing JSON document";

        JsonParseError(std::string const& msg):
            JsonException(msg) {}

        JsonParseError(std::string const& msg, std::size_t pos):
            JsonException(msg + " at position " + std::to_string(pos)) {}
    };

    class JsonTypeError : public JsonException
    {
    public:
        JsonTypeError(std::string const& msg) :
            JsonException(msg) {}
    };

    class JsonDepthLimitExceeded : public JsonException
    {
    public:
        static constexpr char const* DEPTH_LIMIT_EXCEEDED_MESSAGE
            = "Maximum nesting depth of " TO_STRING(MAX_NESTING_DEPTH) " exceeded";

        JsonDepthLimitExceeded(std::size_t pos):
            JsonException(DEPTH_LIMIT_EXCEEDED_MESSAGE + std::string(" at position ") + std::to_string(pos)) {}
    };

    /*
     * end JSON exceptions
     */
}

#endif //JSONPP_JSONEXCEPTION_HPP