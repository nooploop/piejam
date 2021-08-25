// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <mipp.h>

#include <boost/assert.hpp>

#include <concepts>
#include <span>
#include <type_traits>

namespace piejam::audio::simd
{

inline constexpr auto is_aligned = [](auto&& p) {
    return mipp::isAligned(std::forward<decltype(p)>(p));
};

inline constexpr auto clamp =
        []<class T>(mipp::Reg<T> v, mipp::Reg<T> lo, mipp::Reg<T> hi) {
            return mipp::max(lo, mipp::min(hi, v));
        };

template <class T, std::invocable<mipp::Reg<T>> F>
void
transform(std::span<T const> in, T* out, F&& f)
{
    BOOST_ASSERT(is_aligned(in.data()));
    BOOST_ASSERT(is_aligned(out));
    BOOST_ASSERT(in.size() % mipp::N<T>() == 0);

    constexpr auto const step = mipp::N<T>();
    for (auto i = in.data(), e = (i + in.size()); i != e;
         i += step, out += step)
    {
        mipp::Reg<T> reg_in = i;
        mipp::Reg<T> reg_out = f(reg_in);
        reg_out.store(out);
    }
}

template <class T, std::invocable<mipp::Reg<T>, mipp::Reg<T>> F>
void
transform(std::span<T const> in1, T const* in2, T* out, F&& f)
{
    BOOST_ASSERT(is_aligned(in1.data()));
    BOOST_ASSERT(is_aligned(in2));
    BOOST_ASSERT(is_aligned(out));
    BOOST_ASSERT(in1.size() % mipp::N<T>() == 0);

    constexpr auto const step = mipp::N<T>();
    for (auto i1 = in1.data(), e = (i1 + in1.size()); i1 != e;
         i1 += step, in2 += step, out += step)
    {
        mipp::Reg<T> reg_in1 = i1;
        mipp::Reg<T> reg_in2 = in2;
        mipp::Reg<T> reg_out = f(reg_in1, reg_in2);
        reg_out.store(out);
    }
}

namespace detail
{

template <class T>
constexpr auto
reg_data(T const& t) noexcept -> T const&
{
    return t;
}

template <class T>
constexpr auto
reg_data(std::span<T const> const& t) noexcept -> T const*
{
    return t.data();
}

} // namespace detail

template <class T, class I1, class I2>
void
interleave(I1&& in1, I2&& in2, std::span<T> out)
{
    constexpr auto const step = mipp::N<T>();
    constexpr auto const step_out = step * 2;

    BOOST_ASSERT(is_aligned(out.data()));
    BOOST_ASSERT(out.size() >= step_out);
    BOOST_ASSERT(out.size() % step_out == 0);

    constexpr bool const I1_is_span =
            std::is_same_v<std::decay_t<I1>, std::span<T const>>;
    constexpr bool const I2_is_span =
            std::is_same_v<std::decay_t<I2>, std::span<T const>>;

    if constexpr (I1_is_span)
    {
        BOOST_ASSERT(is_aligned(in1.data()));
        BOOST_ASSERT(in1.size() >= step);
        BOOST_ASSERT(in1.size() % step == 0);
        BOOST_ASSERT(in1.size() * 2 == out.size());
    }

    if constexpr (I2_is_span)
    {
        BOOST_ASSERT(is_aligned(in2.data()));
        BOOST_ASSERT(in2.size() >= step);
        BOOST_ASSERT(in2.size() % step == 0);
        BOOST_ASSERT(in2.size() * 2 == out.size());
    }

    if constexpr (I1_is_span && I2_is_span)
    {
        BOOST_ASSERT(in1.size() == in2.size());
    }

    for (auto i1 = detail::reg_data(in1),
              i2 = detail::reg_data(in2),
              o = out.data(),
              e = (o + out.size());
         o < e;
         o += step_out)
    {
        mipp::Reg<T> reg_in1(i1);
        mipp::Reg<T> reg_in2(i2);
        mipp::Regx2<T> reg_out = mipp::interleave(reg_in1, reg_in2);
        reg_out.store(o);

        if constexpr (I1_is_span)
            i1 += step;

        if constexpr (I2_is_span)
            i2 += step;
    }
}

} // namespace piejam::audio::simd
