//
// Created on 2025/10/26.
//

#ifndef JSONPP_JSONEXCEPTION_H
#define JSONPP_JSONEXCEPTION_H
#include <stdexcept>

namespace JSONpp
{
    /*
     * JSON exceptions
     */
    class JSONParseError : public std::runtime_error
    {
    public:
        static constexpr char const* UNPARSABLE_MESSAGE = "Unparsable character(s)";
        static constexpr char const* UNEXPECTED_EOF_MESSAGE = "Unexpected end of file while parsing JSON document";

        // template<typename... Args>
        // JSONParseError(Args... messages):
        //     std::runtime_error((std::string(messages) + ...)) {}

        JSONParseError(std::string const& msg):
            std::runtime_error(msg) {}

        JSONParseError(std::string const& msg, size_t pos):
            std::runtime_error(msg + " at position " + std::to_string(pos)) {}
    };

    class JSONTypeError : public std::runtime_error
    {
    public:
        JSONTypeError(std::string const& msg) :
            std::runtime_error(msg) {}
    };

    class JSONEmptyDataError : public std::runtime_error
    {
    public:
        static constexpr const char* EMPTY_DATA_MESSAGE = "json document is empty";
        JSONEmptyDataError(std::string const& msg = EMPTY_DATA_MESSAGE) :
            std::runtime_error(msg) {}
    };
    /*
     * end JSON exceptions
     */
}

#endif //JSONPP_JSONEXCEPTION_H