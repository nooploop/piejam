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

#include <piejam/audio/engine/slice.h>

#include <algorithm>

namespace piejam::audio::engine
{

namespace detail
{

template <class T>
struct add_visitor
{
    constexpr add_visitor(std::span<T> const& out) noexcept
        : m_out(out)
    {
    }

    constexpr auto operator()(T const l_c, T const r_c) const -> slice<T>
    {
        return l_c + r_c;
    }

    constexpr auto
    operator()(typename slice<T>::span_t const& l_buf, T const r_c) const
            -> slice<T>
    {
        if (r_c != T{})
        {
            BOOST_ASSERT(l_buf.size() == m_out.size());
            std::ranges::transform(l_buf, m_out.begin(), [r_c](T const x) {
                return r_c + x;
            });

            return m_out;
        }
        else
            return l_buf;
    }

    constexpr auto
    operator()(T const l_c, typename slice<T>::span_t const& r_buf) const
            -> slice<T>
    {
        return (*this)(r_buf, l_c);
    }

    constexpr auto operator()(
            typename slice<T>::span_t const& l_buf,
            typename slice<T>::span_t const& r_buf) const -> slice<T>
    {
        BOOST_ASSERT(l_buf.size() == r_buf.size());
        BOOST_ASSERT(l_buf.size() == m_out.size());
        std::transform(
                l_buf.begin(),
                l_buf.end(),
                r_buf.begin(),
                m_out.begin(),
                std::plus<T>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct multiply_visitor
{
    constexpr multiply_visitor(std::span<T> const& out) noexcept
        : m_out(out)
    {
    }

    constexpr auto operator()(T const l_c, T const r_c) const -> slice<T>
    {
        return l_c * r_c;
    }

    constexpr auto
    operator()(typename slice<T>::span_t const& l_buf, T const r_c) const
            -> slice<T>
    {
        if (r_c == T{})
            return T{};
        else if (r_c != T{1})
        {
            BOOST_ASSERT(l_buf.size() == m_out.size());
            std::ranges::transform(l_buf, m_out.begin(), [r_c](T const x) {
                return r_c * x;
            });

            return m_out;
        }
        else
            return l_buf;
    }

    constexpr auto
    operator()(T const l_c, typename slice<T>::span_t const& r_buf) const
            -> slice<T>
    {
        return (*this)(r_buf, l_c);
    }

    constexpr auto operator()(
            typename slice<T>::span_t const& l_buf,
            typename slice<T>::span_t const& r_buf) const -> slice<T>
    {
        BOOST_ASSERT(l_buf.size() == r_buf.size());
        BOOST_ASSERT(l_buf.size() == m_out.size());
        std::transform(
                l_buf.begin(),
                l_buf.end(),
                r_buf.begin(),
                m_out.begin(),
                std::multiplies<T>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct copy_visitor
{
    constexpr copy_visitor(std::span<T> const& out) noexcept
        : m_out(out)
    {
    }

    constexpr void operator()(T const c) const noexcept
    {
        std::ranges::fill(m_out, c);
    }

    constexpr void operator()(std::span<T const> const& buf) const noexcept
    {
        BOOST_ASSERT(buf.size() == m_out.size());
        std::ranges::copy(buf, m_out.begin());
    }

private:
    std::span<T> const m_out;
};

} // namespace detail

template <class T>
auto
add(slice<T> const& l, slice<T> const& r, std::span<T> const& out) noexcept
        -> slice<T>
{
    return std::visit(
            detail::add_visitor<T>(out),
            l.as_variant(),
            r.as_variant());
}

template <class T>
auto
multiply(slice<T> const& l, slice<T> const& r, std::span<T> const& out) noexcept
        -> slice<T>
{
    return std::visit(
            detail::multiply_visitor<T>(out),
            l.as_variant(),
            r.as_variant());
}

template <class T>
void
copy(slice<T> const& s, std::span<T> const& out) noexcept
{
    std::visit(detail::copy_visitor(out), s.as_variant());
}

} // namespace piejam::audio::engine
