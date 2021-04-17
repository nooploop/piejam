// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <mipp.h>

#include <boost/assert.hpp>

#include <concepts>
#include <span>

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

template <class T>
void
interleave(std::span<T const> in1, T const* in2, T* out)
{
    BOOST_ASSERT(is_aligned(in1.data()));
    BOOST_ASSERT(is_aligned(in2));
    BOOST_ASSERT(is_aligned(out));
    BOOST_ASSERT(in1.size() % mipp::N<T>() == 0);

    constexpr auto const step = mipp::N<T>();
    constexpr auto const step_out = step * 2;
    for (auto i1 = in1.data(), e = (i1 + in1.size()); i1 != e;
         i1 += step, in2 += step, out += step_out)
    {
        mipp::Reg<T> reg_in1 = i1;
        mipp::Reg<T> reg_in2 = in2;
        mipp::Regx2<T> reg_out = mipp::interleave(reg_in1, reg_in2);
        reg_out.store(out);
    }
}

} // namespace piejam::audio::simd
