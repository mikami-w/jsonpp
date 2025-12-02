#ifndef JSONPP_JSON_SERIALIZE_HANDLER_HPP
#define JSONPP_JSON_SERIALIZE_HANDLER_HPP

#include <string>

namespace JSONpp
{
    namespace details
    {
        class StringSerializeHandler
        {
            std::string& buffer;

        public:
            explicit StringSerializeHandler(std::string& buf): buffer(buf) {}

            void append(char ch)
            {
                buffer.push_back(ch);
            }

            void append(std::string_view str)
            {
                buffer.append(str);
            }

            void append(char const* cstr, std::size_t length)
            {
                buffer.append(cstr, length);
            }
        };

        class OStreamSerializeHandler
        {
            std::ostream& out;

        public:
            explicit OStreamSerializeHandler(std::ostream& os): out(os) {}

            void append(char ch)
            {
                out.put(ch);
            }

            void append(std::string_view str)
            {
                out.write(str.data(), str.size());
            }

            void append(char const* cstr, std::size_t length)
            {
                out.write(cstr, length);
            }

            bool bad() const
            {
                return out.bad();
            }
        };

    }
}

#endif //JSONPP_JSON_SERIALIZE_HANDLER_HPP