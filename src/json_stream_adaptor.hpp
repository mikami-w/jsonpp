#ifndef JSONPP_JSON_STREAM_ADAPTOR_HPP
#define JSONPP_JSON_STREAM_ADAPTOR_HPP

#include <cstddef>
#include <algorithm>
#include <cassert>
#include <type_traits>
#include <string_view>

namespace JSONpp
{
    class StringViewStream
    {
        std::string_view data;
        std::size_t pos;

    public:
        int peek() const noexcept { if (pos < data.size()) return static_cast<unsigned char>(data[pos]); else return EOF; } // To ensure that 255 is correctly handled
        int advance() noexcept { if (pos < data.size()) return static_cast<unsigned char>(data[pos++]); else return EOF; }
        std::size_t tell_pos() const noexcept { return pos; }
        bool eof() const noexcept { return pos >= data.size(); }

        std::size_t size() const noexcept { return data.size(); }

        void seek(std::size_t step) noexcept
        {
            assert(pos + step <= data.size() && "StringViewStream::seek out of bounds!");
            pos += step;
        }

        std::string_view get_chunk(std::size_t begin, std::size_t length) const noexcept
        {
            assert(begin <= data.size() && "StringViewStream::get_chunk invalid begin!");
            return data.substr(begin, length);
        }

        template <typename FunctorT>
        std::string_view read_chunk_until(FunctorT predicate) &
        {
            std::string_view remaining = data.substr(pos);
            auto it = std::find_if(remaining.begin(), remaining.end(), predicate);
            auto chunk_size = std::distance(remaining.begin(), it);
            // Get chunk (if chunk_size=0, return empty view)
            std::string_view chunk = data.substr(pos, chunk_size);

            pos += chunk_size;

            return chunk;
        }
        explicit StringViewStream(std::string_view doc): data(doc), pos(0) {}
    };

    class IStreamStream
    {
        std::istream& is;
        std::size_t pos;
    public:
        int peek() const { return is.peek(); }
        int advance()
        {
            auto c = is.get();
            if (c != EOF)
                ++pos;
            return c;
        }
        std::size_t tell_pos() const noexcept { return pos; }
        bool eof() const { return is.peek() == EOF; }

        explicit IStreamStream(std::istream& _is): is(_is), pos(0) { is.unsetf(std::ios::skipws); }
    };
}
#endif //JSONPP_JSON_STREAM_ADAPTOR_HPP