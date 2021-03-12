// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/slice.h>

#include <xsimd/math/xsimd_basic_math.hpp>
#include <xsimd/stl/algorithms.hpp>

#include <boost/align/is_aligned.hpp>
#include <boost/assert.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

namespace detail
{

template <class T>
struct slice_add
{
    constexpr slice_add(std::span<T> const& out) noexcept
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
            BOOST_ASSERT(boost::alignment::is_aligned(
                    l_buf.data(),
                    XSIMD_DEFAULT_ALIGNMENT));
            BOOST_ASSERT(boost::alignment::is_aligned(
                    m_out.data(),
                    XSIMD_DEFAULT_ALIGNMENT));
            BOOST_ASSERT(l_buf.size_bytes() % XSIMD_DEFAULT_ALIGNMENT == 0);
            xsimd::transform(
                    l_buf.begin(),
                    l_buf.end(),
                    m_out.begin(),
                    [r_c](auto&& x) { return r_c + x; });

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
        BOOST_ASSERT(boost::alignment::is_aligned(
                l_buf.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(boost::alignment::is_aligned(
                r_buf.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(boost::alignment::is_aligned(
                m_out.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(l_buf.size_bytes() % XSIMD_DEFAULT_ALIGNMENT == 0);
        xsimd::transform(
                l_buf.begin(),
                l_buf.end(),
                r_buf.begin(),
                m_out.begin(),
                std::plus<>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct slice_multiply
{
    constexpr slice_multiply(std::span<T> const& out) noexcept
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
            BOOST_ASSERT(boost::alignment::is_aligned(
                    l_buf.data(),
                    XSIMD_DEFAULT_ALIGNMENT));
            BOOST_ASSERT(boost::alignment::is_aligned(
                    m_out.data(),
                    XSIMD_DEFAULT_ALIGNMENT));
            BOOST_ASSERT(l_buf.size_bytes() % XSIMD_DEFAULT_ALIGNMENT == 0);
            xsimd::transform(
                    l_buf.begin(),
                    l_buf.end(),
                    m_out.begin(),
                    [r_c](auto&& x) { return r_c * x; });

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
        BOOST_ASSERT(boost::alignment::is_aligned(
                l_buf.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(boost::alignment::is_aligned(
                r_buf.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(boost::alignment::is_aligned(
                m_out.data(),
                XSIMD_DEFAULT_ALIGNMENT));
        BOOST_ASSERT(l_buf.size_bytes() % XSIMD_DEFAULT_ALIGNMENT == 0);
        xsimd::transform(
                l_buf.begin(),
                l_buf.end(),
                r_buf.begin(),
                m_out.begin(),
                std::multiplies<>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct slice_clip
{
    constexpr slice_clip(T min, T max, std::span<T> const& out) noexcept
        : m_min(min)
        , m_max(max)
        , m_out(out)
    {
        BOOST_ASSERT(min <= max);
    }

    constexpr auto operator()(T const c) const -> slice<T>
    {
        return std::clamp(c, m_min, m_max);
    }

    constexpr auto operator()(typename slice<T>::span_t const& buf) const
            -> slice<T>
    {
        xsimd::transform(
                buf.begin(),
                buf.end(),
                m_out.begin(),
                [min = m_min, max = m_max](auto&& x) {
                    using x_t = std::decay_t<decltype(x)>;
                    return xsimd::clip(
                            std::forward<decltype(x)>(x),
                            x_t{min},
                            x_t{max});
                });
        return m_out;
    }

private:
    T m_min;
    T m_max;
    std::span<T> const m_out;
};

template <class T>
struct slice_copy
{
    constexpr slice_copy(std::span<T> const& out) noexcept
        : m_out(out)
    {
    }

    constexpr void operator()(T const c) const noexcept
    {
        std::ranges::fill(m_out, c);
    }

    constexpr void operator()(std::span<T const> const& buf) const noexcept
    {
        if (buf.data() != m_out.data())
        {
            BOOST_ASSERT(buf.size() == m_out.size());
            std::ranges::copy(buf, m_out.begin());
        }
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct subslice
{
    constexpr subslice(std::size_t const offset, std::size_t size) noexcept
        : m_offset(offset)
        , m_size(size)
    {
    }

    constexpr auto operator()(T const c) const noexcept -> slice<T>
    {
        return c;
    }

    constexpr auto operator()(std::span<T const> const& buf) const noexcept
            -> slice<T>
    {
        BOOST_ASSERT(m_offset < buf.size());
        BOOST_ASSERT(m_offset + m_size <= buf.size());
        return std::span<T const>(buf.data() + m_offset, m_size);
    }

private:
    std::size_t const m_offset;
    std::size_t const m_size;
};

template <class T, class F>
struct slice_transform
{
    template <std::convertible_to<F> G>
    constexpr slice_transform(std::span<T> const& out, G&& g)
        : m_out(out)
        , m_f(std::forward<G>(g))
    {
    }

    constexpr auto operator()(T const c) const noexcept -> slice<T>
    {
        return m_f(c);
    }

    constexpr auto operator()(std::span<T const> const& buf) const noexcept
            -> slice<T>
    {
        BOOST_ASSERT(buf.size() == m_out.size());
        //        BOOST_ASSERT(boost::alignment::is_aligned(
        //                buf.data(),
        //                XSIMD_DEFAULT_ALIGNMENT));
        //        BOOST_ASSERT(boost::alignment::is_aligned(
        //                m_out.data(),
        //                XSIMD_DEFAULT_ALIGNMENT));
        //        BOOST_ASSERT(buf.size_bytes() % XSIMD_DEFAULT_ALIGNMENT == 0);
        xsimd::transform(buf.begin(), buf.end(), m_out.begin(), m_f);
        return m_out;
    }

private:
    std::span<T> const m_out;
    F m_f;
};

} // namespace detail

template <class T>
constexpr auto
add(slice<T> const& l, slice<T> const& r, std::span<T> const& out) noexcept
        -> slice<T>
{
    return std::visit(
            detail::slice_add<T>(out),
            l.as_variant(),
            r.as_variant());
}

template <class T>
constexpr auto
multiply(slice<T> const& l, slice<T> const& r, std::span<T> const& out) noexcept
        -> slice<T>
{
    return std::visit(
            detail::slice_multiply<T>(out),
            l.as_variant(),
            r.as_variant());
}

template <class T>
constexpr auto
clip(slice<T> const& s, T min, T max, std::span<T> const& out) noexcept
        -> slice<T>
{
    return std::visit(detail::slice_clip<T>(min, max, out), s.as_variant());
}

template <class T>
constexpr void
copy(slice<T> const& s, std::span<T> const& out) noexcept
{
    std::visit(detail::slice_copy(out), s.as_variant());
}

template <class T, class F>
constexpr auto
transform(slice<T> const& s, std::span<T> const& out, F&& f) noexcept
        -> slice<T>
{
    return std::visit(
            detail::slice_transform<T, F>(out, std::forward<F>(f)),
            s.as_variant());
}

template <class T>
constexpr auto
subslice(
        slice<T> const& s,
        std::size_t const offset,
        std::size_t const size) noexcept -> slice<T>
{
    return std::visit(detail::subslice<T>(offset, size), s.as_variant());
}

} // namespace piejam::audio::engine
