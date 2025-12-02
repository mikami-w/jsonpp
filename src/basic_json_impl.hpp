#ifndef JSONPP_BASIC_JSON_IMPL_HPP
#define JSONPP_BASIC_JSON_IMPL_HPP

#include "macro_def.hpp"
#include "basic_json.hpp"
#include "json_serializer.hpp"

namespace JSONpp
{
    using namespace traits;

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::operator[](std::size_t index)
    {
        auto& arr = as_array();
        assert(index < arr.size() && "JSON array index out of range");
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::operator[](std::size_t index) const
    {
        auto const& arr = as_array();
        assert(index < arr.size() && "JSON array index out of range");
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::at(std::size_t index)
    {
        auto& arr = as_array();
        if (index >= arr.size())
            throw JsonTypeError("JSON array index out of range");
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::at(std::size_t index) const
    {
        auto const& arr = as_array();
        if (index >= arr.size())
            throw JsonTypeError("JSON array index out of range");
        return arr[index];
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::operator[](std::string const& key) const
    {
        auto const& obj = as_object();
        auto it = obj.find(key);
        if (it == obj.end())
            throw JsonTypeError("Key \"" + key + "\" not found in JSON object");
        return it->second;
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE& BASIC_JSON_TYPE::at(std::string const& key)
    {
        auto& obj = as_object();
        auto it = obj.find(key);
        if (it == obj.end())
            throw JsonTypeError("Key \"" + key + "\" not found in JSON object");
        return it->second;
    }

    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE const& BASIC_JSON_TYPE::at(std::string const& key) const
    {
        auto const& obj = as_object();
        auto it = obj.find(key);
        if (it == obj.end())
            throw JsonTypeError("Key \"" + key + "\" not found in JSON object");
        return it->second;
    }

    BASIC_JSON_TEMPLATE
    bool BASIC_JSON_TYPE::operator==(BASIC_JSON_TYPE const& other) const
    {
        if (value.index() != other.value.index()) return false;
        return value == other.value;
    }

    /*
     * Parse a document to JsonType, accessing data with std::string_view.
     */
    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(std::string_view json_doc)
    {
        details::StringViewStream svs(json_doc);
        return details::Parser<details::StringViewStream, basic_json>(svs).parse();
    }

    /*
     * Parse a document to JsonType, accessing data with std::istream.
     */
    BASIC_JSON_TEMPLATE
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(std::istream& json_istream)
    {
        details::IStreamStream iss(json_istream);
        return details::Parser<details::IStreamStream, basic_json>(iss).parse();
    }

    /*
     * Parse a document to JsonType, accessing data with a JSON Stream.
     * Data should have been provided to the stream before calling this function.
     */
    BASIC_JSON_TEMPLATE
    template <typename StreamT,
        std::enable_if_t<traits::isJsonStream_v<StreamT>, int>>
    BASIC_JSON_TYPE BASIC_JSON_TYPE::parse(StreamT& stream)
    {
        return details::Parser<StreamT, basic_json>(stream).parse();
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::dump(std::string& buffer) const
    {
        details::StringSerializeHandler ssh(buffer);
        details::JsonSerializer<basic_json, details::StringSerializeHandler>
            serializer(ssh);
        serializer.dump(*this);
    }

    BASIC_JSON_TEMPLATE
    void BASIC_JSON_TYPE::dump(std::ostream& os) const
    {
        details::OStreamSerializeHandler ossh(os);
        details::JsonSerializer<basic_json, details::OStreamSerializeHandler>
            serializer(ossh);
        serializer.dump(*this);
    }

    BASIC_JSON_TEMPLATE
    template <typename SerializeHandlerT,
        std::enable_if_t<traits::isJsonSerializeHandler_v<SerializeHandlerT>, int>>
    void BASIC_JSON_TYPE::dump(SerializeHandlerT& handler)
    {
        details::JsonSerializer<basic_json, SerializeHandlerT> serializer(handler);
        serializer.dump(*this);
    }

    /*
     * end asserted accessor
     */
}


#endif //JSONPP_BASIC_JSON_IMPL_HPP