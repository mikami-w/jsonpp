/*
jsonpp - A modern, header-only C++ JSON library
Copyright 2025-2026 Mikami (jsonpp project)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#ifndef JSONPP_JSON_ITERATOR_HPP
#define JSONPP_JSON_ITERATOR_HPP

#include "jsonexception.hpp"

#include <cassert>
#include <cstdint>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace jsonpp::details
{
    template <typename T, typename = void>
    struct has_pre_decrement : std::false_type {};

    template <typename T>
    struct has_pre_decrement<T, std::void_t<decltype(--std::declval<T&>())>> : std::true_type {};

    template <typename JsonT, bool IsConst>
    class JsonIterator
    {
    private:
        template <bool B>
        using _json_ptr = std::conditional_t<B, JsonT const*, JsonT*>;

        template <bool B>
        using _array_it = std::conditional_t<B,
                                             typename JsonT::array::const_iterator,
                                             typename JsonT::array::iterator>;

        template <bool B>
        using _object_it = std::conditional_t<B,
                                              typename JsonT::object::const_iterator,
                                              typename JsonT::object::iterator>;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = JsonT;
        using pointer = _json_ptr<IsConst>;
        using reference = std::conditional_t<IsConst, JsonT const&, JsonT&>;
        using iterator_category = std::bidirectional_iterator_tag;

        using array_iterator = _array_it<IsConst>;
        using object_iterator = _object_it<IsConst>;

    private:
        enum class Mode : std::uint8_t
        {
            empty,
            scalar,
            array,
            object
        };

    public:
        static constexpr bool supports_array_reverse = has_pre_decrement<array_iterator>::value;
        static constexpr bool supports_object_reverse = has_pre_decrement<object_iterator>::value;
        static constexpr bool supports_reverse = supports_array_reverse && supports_object_reverse;

    public:
        JsonIterator() = default;

        template <bool B = IsConst, std::enable_if_t<B, int> = 0>
        JsonIterator(JsonIterator<JsonT, false> const& other):
            m_owner(other.m_owner),
            m_mode(other.m_mode),
            m_scalar_is_end(other.m_scalar_is_end),
            m_array_iter(other.m_array_iter),
            m_object_iter(other.m_object_iter)
        {}

        static JsonIterator make_empty(pointer owner)
        {
            JsonIterator it;
            it.m_owner = owner;
            it.m_mode = Mode::empty;
            it.m_scalar_is_end = true;
            return it;
        }

        static JsonIterator make_scalar(pointer owner, bool at_end)
        {
            JsonIterator it;
            it.m_owner = owner;
            it.m_mode = Mode::scalar;
            it.m_scalar_is_end = at_end;
            return it;
        }

        static JsonIterator make_array(pointer owner, array_iterator it_value)
        {
            JsonIterator it;
            it.m_owner = owner;
            it.m_mode = Mode::array;
            it.m_array_iter = std::move(it_value);
            return it;
        }

        static JsonIterator make_object(pointer owner, object_iterator it_value)
        {
            JsonIterator it;
            it.m_owner = owner;
            it.m_mode = Mode::object;
            it.m_object_iter = std::move(it_value);
            return it;
        }

        reference operator*() const
        {
            assert(m_owner != nullptr && "Cannot dereference default-constructed iterator");
            switch (m_mode)
            {
            case Mode::scalar:
                assert(!m_scalar_is_end && "Cannot dereference end iterator");
                return *m_owner;
            case Mode::array:
            {
                auto const& arr = m_owner->as_array();
                assert(m_array_iter != arr.end() && "Cannot dereference end iterator");
                return *m_array_iter;
            }
            case Mode::object:
            {
                auto const& obj = m_owner->as_object();
                assert(m_object_iter != obj.end() && "Cannot dereference end iterator");
                return m_object_iter->second;
            }
            case Mode::empty:
                assert(false && "Cannot dereference iterator for empty JSON value");
#if defined(__GNUC__) || defined(__clang__)
                __builtin_unreachable();
#elif defined(_MSC_VER)
                __assume(0);
#endif
            }

#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
        }

        pointer operator->() const
        {
            return std::addressof(operator*());
        }

        JsonIterator& operator++()
        {
            assert(m_owner != nullptr && "Cannot increment default-constructed iterator");
            switch (m_mode)
            {
            case Mode::scalar:
                assert(!m_scalar_is_end && "Cannot increment end iterator");
                m_scalar_is_end = true;
                return *this;
            case Mode::array:
                ++m_array_iter;
                return *this;
            case Mode::object:
                ++m_object_iter;
                return *this;
            case Mode::empty:
                assert(false && "Cannot increment iterator for empty JSON value");
                return *this;
            }

#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
        }

        JsonIterator operator++(int)
        {
            JsonIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        JsonIterator& operator--()
        {
            assert(m_owner != nullptr && "Cannot decrement default-constructed iterator");
            switch (m_mode)
            {
            case Mode::scalar:
                assert(m_scalar_is_end && "Cannot decrement begin iterator");
                m_scalar_is_end = false;
                return *this;
            case Mode::array:
                if constexpr (supports_array_reverse)
                {
                    --m_array_iter;
                    return *this;
                }
                else
                {
                    throw JsonTypeError("Array iterator does not support decrement");
                }
            case Mode::object:
                if constexpr (supports_object_reverse)
                {
                    --m_object_iter;
                    return *this;
                }
                else
                {
                    throw JsonTypeError("Object iterator does not support decrement");
                }
            case Mode::empty:
                assert(false && "Cannot decrement iterator for empty JSON value");
                return *this;
            }

#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
        }

        JsonIterator operator--(int)
        {
            JsonIterator tmp(*this);
            --(*this);
            return tmp;
        }

        template <bool OtherConst>
        bool operator==(JsonIterator<JsonT, OtherConst> const& rhs) const
        {
#ifndef NDEBUG
            if (m_owner != nullptr && rhs.m_owner != nullptr)
            {
                assert(m_owner == rhs.m_owner && "Comparing iterators from different JSON values is undefined");
                assert(m_mode == rhs.m_mode && "Comparing iterators in different modes is undefined");
            }
#endif

            if (m_owner != rhs.m_owner || m_mode != rhs.m_mode)
                return false;

            switch (m_mode)
            {
            case Mode::empty:
                return true;
            case Mode::scalar:
                return m_scalar_is_end == rhs.m_scalar_is_end;
            case Mode::array:
                return m_array_iter == rhs.m_array_iter;
            case Mode::object:
                return m_object_iter == rhs.m_object_iter;
            }

#if defined(__GNUC__) || defined(__clang__)
            __builtin_unreachable();
#elif defined(_MSC_VER)
            __assume(0);
#endif
        }

        template <bool OtherConst>
        bool operator!=(JsonIterator<JsonT, OtherConst> const& rhs) const
        {
            return !(*this == rhs);
        }

        typename JsonT::string const& key() const
        {
            if (m_mode != Mode::object)
                throw JsonTypeError("Iterator key() is only valid for object iterators");
            return m_object_iter->first;
        }

    private:
        pointer m_owner = nullptr;
        Mode m_mode = Mode::empty;
        bool m_scalar_is_end = true;
        array_iterator m_array_iter{};
        object_iterator m_object_iter{};

        friend class JsonIterator<JsonT, !IsConst>;
        template <typename, bool>
        friend class JsonIterator;
    };
}
#endif //JSONPP_JSON_ITERATOR_HPP