#ifndef JSONPP_JSONEXCEPTION_HPP
#define JSONPP_JSONEXCEPTION_HPP
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

        JSONParseError(std::string const& msg):
            std::runtime_error(msg) {}

        JSONParseError(std::string const& msg, std::size_t pos):
            std::runtime_error(msg + " at position " + std::to_string(pos)) {}
    };

    class JSONTypeError : public std::runtime_error
    {
    public:
        JSONTypeError(std::string const& msg) :
            std::runtime_error(msg) {}
    };

    /*
     * end JSON exceptions
     */
}

#endif //JSONPP_JSONEXCEPTION_HPP