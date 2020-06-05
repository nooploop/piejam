// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <array>
#include <cassert>
#include <iterator>
#include <type_traits>

namespace piejam::range
{

template <class T>
class span
{
public:
    using element_type = T;
    using value_type = std::remove_cv_t<T>;
    using index_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr span() noexcept
        : m_data(nullptr)
        , m_size(0)
    {
    }

    constexpr span(pointer ptr, index_type count)
        : m_data(ptr)
        , m_size(count)
    {
    }

    constexpr span(pointer first, pointer last)
        : span(first, last - first)
    {
    }

    template <std::size_t N>
    constexpr span(element_type (&arr)[N]) noexcept
        : m_data(std::data(arr))
        , m_size(N)
    {
    }

    template <std::size_t N>
    constexpr span(std::array<value_type, N>& arr) noexcept
        : m_data(arr.data())
        , m_size(N)
    {
    }

    template <std::size_t N>
    constexpr span(std::array<value_type, N> const& arr) noexcept
        : m_data(arr.data())
        , m_size(N)
    {
    }

    constexpr span(span const&) noexcept = default;

    constexpr auto begin() const noexcept -> iterator
    {
        assert(m_data);
        return m_data;
    }

    constexpr auto cbegin() const noexcept -> const_iterator
    {
        assert(m_data);
        return m_data;
    }

    constexpr auto end() const noexcept -> iterator
    {
        assert(m_data);
        return m_data + m_size;
    }

    constexpr auto cend() const noexcept -> const_iterator
    {
        assert(m_data);
        return m_data + m_size;
    }

    constexpr auto rbegin() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(end());
    }

    constexpr auto crbegin() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cend());
    }

    constexpr auto rend() const noexcept -> reverse_iterator
    {
        return std::make_reverse_iterator(begin());
    }

    constexpr auto crend() const noexcept -> const_reverse_iterator
    {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr auto front() const -> reference { return *begin(); }

    constexpr auto back() const -> reference { return *(end() - 1); }

    constexpr auto data() const noexcept -> pointer { return m_data; }

    constexpr auto operator[](index_type idx) const -> reference
    {
        assert(data());
        return data()[idx];
    }

    constexpr auto size() const noexcept -> index_type { return m_size; }

    constexpr auto size_bytes() const noexcept -> index_type
    {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

private:
    pointer m_data{};
    std::size_t m_size{};
};

} // namespace piejam::range
