// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/slice.h>

#include <piejam/math.h>
#include <piejam/numeric/mipp_iterator.h>
#include <piejam/switch_cast.h>

#include <boost/assert.hpp>
#include <boost/hof/capture.hpp>

#include <algorithm>

namespace piejam::audio
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

            std::transform(
                    numeric::mipp_begin(l_buf),
                    numeric::mipp_end(l_buf),
                    numeric::mipp_begin(m_out),
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

        std::transform(
                numeric::mipp_begin(l_buf),
                numeric::mipp_end(l_buf),
                numeric::mipp_begin(r_buf),
                numeric::mipp_begin(m_out),
                std::plus<>{});

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

                std::transform(
                        numeric::mipp_begin(l_buf),
                        numeric::mipp_end(l_buf),
                        numeric::mipp_begin(m_out),
                        bhof::capture(mipp::Reg<T>(T{}))(std::minus<>{}));
                return m_out;

            default:
                BOOST_ASSERT(l_buf.size() == m_out.size());

                std::transform(
                        numeric::mipp_begin(l_buf),
                        numeric::mipp_end(l_buf),
                        numeric::mipp_begin(m_out),
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

        std::transform(
                numeric::mipp_begin(l_buf),
                numeric::mipp_end(l_buf),
                numeric::mipp_begin(r_buf),
                numeric::mipp_begin(m_out),
                std::multiplies<>{});

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
        BOOST_ASSERT(mipp::isAligned(out.data()));
    }

    constexpr auto
    operator()(typename slice<T>::constant_t const c) const -> slice<T>
    {
        return math::clamp(c, m_min, m_max);
    }

    constexpr auto
    operator()(typename slice<T>::span_t const buf) const -> slice<T>
    {
        BOOST_ASSERT(mipp::isAligned(buf.data()));
        std::transform(
                numeric::mipp_begin(buf),
                numeric::mipp_end(buf),
                numeric::mipp_begin(m_out),
                [min = mipp::Reg<T>(m_min),
                 max = mipp::Reg<T>(m_max)](mipp::Reg<T> const x) {
                    return mipp::max(min, mipp::min(max, x));
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

} // namespace detail

template <class T>
constexpr auto
add(slice<T> const& l,
    slice<T> const& r,
    std::span<T> const out) noexcept -> slice<T>
{
    return visit(detail::slice_add<T>(out), l, r);
}

template <class T>
constexpr auto
multiply(slice<T> const& l, slice<T> const& r, std::span<T> const out) noexcept
        -> slice<T>
{
    return visit(detail::slice_multiply<T>(out), l, r);
}

template <class T>
constexpr auto
clamp(slice<T> const& s,
      T const min,
      T const max,
      std::span<T> const out) noexcept -> slice<T>
{
    return visit(detail::slice_clamp<T>(min, max, out), s);
}

template <class T>
constexpr void
copy(slice<T> const& s, std::span<T> const out) noexcept
{
    visit(detail::slice_copy(out), s);
}

template <class T>
constexpr auto
subslice(
        slice<T> const& s,
        std::size_t const offset,
        std::size_t const size) noexcept -> slice<T>
{
    return visit(detail::subslice<T>(offset, size), s);
}

} // namespace piejam::audio
