// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/slice.h>

#include <piejam/audio/simd.h>
#include <piejam/math.h>
#include <piejam/switch_cast.h>

#include <boost/assert.hpp>
#include <boost/hof/capture.hpp>

#include <algorithm>

namespace piejam::audio::engine
{

namespace detail
{

namespace bhof = boost::hof;

template <class T>
struct slice_add
{
    constexpr slice_add(std::span<T> const out) noexcept
        : m_out(out)
    {
    }

    constexpr auto operator()(
            typename slice<T>::constant_t const l_c,
            typename slice<T>::constant_t const r_c) const -> slice<T>
    {
        return l_c + r_c;
    }

    constexpr auto operator()(
            typename slice<T>::span_t const l_buf,
            typename slice<T>::constant_t const r_c) const -> slice<T>
    {
        if (r_c != T{})
        {
            BOOST_ASSERT(l_buf.size() == m_out.size());
            simd::transform(
                    l_buf,
                    m_out.data(),
                    bhof::capture(mipp::Reg<T>(r_c))(std::plus<>{}));

            return m_out;
        }
        else
        {
            return l_buf;
        }
    }

    constexpr auto operator()(
            typename slice<T>::constant_t const l_c,
            typename slice<T>::span_t const r_buf) const -> slice<T>
    {
        return (*this)(r_buf, l_c);
    }

    constexpr auto operator()(
            typename slice<T>::span_t const l_buf,
            typename slice<T>::span_t const r_buf) const -> slice<T>
    {
        BOOST_ASSERT(l_buf.size() == r_buf.size());
        BOOST_ASSERT(l_buf.size() == m_out.size());
        simd::transform(l_buf, r_buf.data(), m_out.data(), std::plus<>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct slice_multiply
{
    constexpr slice_multiply(std::span<T> const out) noexcept
        : m_out(out)
    {
    }

    constexpr auto operator()(
            typename slice<T>::constant_t const l_c,
            typename slice<T>::constant_t const r_c) const -> slice<T>
    {
        return l_c * r_c;
    }

    constexpr auto operator()(
            typename slice<T>::span_t const l_buf,
            typename slice<T>::constant_t const r_c) const -> slice<T>
    {
        switch (switch_cast(r_c))
        {
            case switch_cast(T{0}):
                return r_c;

            case switch_cast(T{1}):
                return l_buf;

            case switch_cast(T{-1}):
                BOOST_ASSERT(l_buf.size() == m_out.size());
                simd::transform(
                        l_buf,
                        m_out.data(),
                        bhof::capture(mipp::Reg<T>(T{}))(std::minus<>{}));
                return m_out;

            default:
                BOOST_ASSERT(l_buf.size() == m_out.size());
                simd::transform(
                        l_buf,
                        m_out.data(),
                        bhof::capture(mipp::Reg<T>(r_c))(std::multiplies<>{}));

                return m_out;
        }
    }

    constexpr auto operator()(
            typename slice<T>::constant_t const l_c,
            typename slice<T>::span_t const r_buf) const -> slice<T>
    {
        return (*this)(r_buf, l_c);
    }

    constexpr auto operator()(
            typename slice<T>::span_t const l_buf,
            typename slice<T>::span_t const r_buf) const -> slice<T>
    {
        BOOST_ASSERT(l_buf.size() == r_buf.size());
        BOOST_ASSERT(l_buf.size() == m_out.size());
        simd::transform(l_buf, r_buf.data(), m_out.data(), std::multiplies<>{});
        return m_out;
    }

private:
    std::span<T> const m_out;
};

template <class T>
struct slice_clamp
{
    constexpr slice_clamp(
            T const min,
            T const max,
            std::span<T> const out) noexcept
        : m_min(min)
        , m_max(max)
        , m_out(out)
    {
        BOOST_ASSERT(min <= max);
        BOOST_ASSERT(simd::is_aligned(out.data()));
    }

    constexpr auto operator()(typename slice<T>::constant_t const c) const
            -> slice<T>
    {
        return math::clamp(c, m_min, m_max);
    }

    constexpr auto operator()(typename slice<T>::span_t const buf) const
            -> slice<T>
    {
        BOOST_ASSERT(simd::is_aligned(buf.data()));
        simd::transform(
                buf,
                m_out.data(),
                [min = mipp::Reg<T>(m_min),
                 max = mipp::Reg<T>(m_max)](auto&& x) {
                    return simd::clamp(std::forward<decltype(x)>(x), min, max);
                });
        return m_out;
    }

private:
    T const m_min;
    T const m_max;
    std::span<T> const m_out;
};

template <class T>
struct slice_copy
{
    constexpr slice_copy(std::span<T> const out) noexcept
        : m_out(out)
    {
    }

    constexpr void
    operator()(typename slice<T>::constant_t const c) const noexcept
    {
        std::ranges::fill(m_out, c);
    }

    constexpr void
    operator()(typename slice<T>::span_t const buf) const noexcept
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

    constexpr auto
    operator()(typename slice<T>::constant_t const c) const noexcept -> slice<T>
    {
        return c;
    }

    constexpr auto
    operator()(typename slice<T>::span_t const buf) const noexcept -> slice<T>
    {
        BOOST_ASSERT(m_offset < buf.size());
        BOOST_ASSERT(m_offset + m_size <= buf.size());
        return buf.subspan(m_offset, m_size);
    }

private:
    std::size_t const m_offset;
    std::size_t const m_size;
};

template <class T, class F>
struct slice_transform
{
    template <std::convertible_to<F> G>
    constexpr slice_transform(std::span<T> const out, G&& g)
        : m_out(out)
        , m_f(std::forward<G>(g))
    {
    }

    constexpr auto
    operator()(typename slice<T>::constant_t const c) const noexcept -> slice<T>
    {
        return m_f(c);
    }

    constexpr auto
    operator()(typename slice<T>::span_t const buf) const noexcept -> slice<T>
    {
        BOOST_ASSERT(buf.size() == m_out.size());
        simd::transform(buf, m_out.data(), m_f);
        return m_out;
    }

private:
    std::span<T> const m_out;
    F m_f;
};

template <class T>
struct slice_interleave
{
    constexpr slice_interleave(std::span<T> const out) noexcept
        : m_out(out)
    {
    }

    template <class I1, class I2>
    constexpr void operator()(I1&& l, I2&& r) const noexcept
    {
        simd::interleave(std::forward<I1>(l), std::forward<I2>(r), m_out);
    }

    template <class I1, class I2, class I3, class I4>
    constexpr void operator()(I1&& i1, I2&& i2, I3&& i3, I4&& i4) const noexcept
    {
        simd::interleave(
                std::forward<I1>(i1),
                std::forward<I2>(i2),
                std::forward<I3>(i3),
                std::forward<I4>(i4),
                m_out);
    }

private:
    std::span<T> const m_out;
};

} // namespace detail

template <class T>
constexpr auto
add(slice<T> const& l, slice<T> const& r, std::span<T> const out) noexcept
        -> slice<T>
{
    return slice<T>::visit(detail::slice_add<T>(out), l, r);
}

template <class T>
constexpr auto
multiply(slice<T> const& l, slice<T> const& r, std::span<T> const out) noexcept
        -> slice<T>
{
    return slice<T>::visit(detail::slice_multiply<T>(out), l, r);
}

template <class T>
constexpr auto
clamp(slice<T> const& s,
      T const min,
      T const max,
      std::span<T> const out) noexcept -> slice<T>
{
    return slice<T>::visit(detail::slice_clamp<T>(min, max, out), s);
}

template <class T>
constexpr void
copy(slice<T> const& s, std::span<T> const out) noexcept
{
    slice<T>::visit(detail::slice_copy(out), s);
}

template <class T, class F>
constexpr auto
transform(slice<T> const& s, std::span<T> const out, F&& f) noexcept -> slice<T>
{
    return slice<T>::visit(
            detail::slice_transform<T, F>(out, std::forward<F>(f)),
            s);
}

template <class T>
constexpr auto
subslice(
        slice<T> const& s,
        std::size_t const offset,
        std::size_t const size) noexcept -> slice<T>
{
    return slice<T>::visit(detail::subslice<T>(offset, size), s);
}

template <class T>
constexpr auto
interleave(
        slice<T> const& s1,
        slice<T> const& s2,
        std::span<T> const out) noexcept
{
    slice<T>::visit(detail::slice_interleave(out), s1, s2);
}

template <class T>
constexpr auto
interleave(
        slice<T> const& s1,
        slice<T> const& s2,
        slice<T> const& s3,
        slice<T> const& s4,
        std::span<T> const out) noexcept
{
    slice<T>::visit(detail::slice_interleave(out), s1, s2, s3, s4);
}

} // namespace piejam::audio::engine
