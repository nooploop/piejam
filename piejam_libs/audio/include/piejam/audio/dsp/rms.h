// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <mipp.h>

#include <boost/assert.hpp>

#include <cmath>
#include <span>

namespace piejam::audio::dsp
{

template <std::floating_point T>
auto
rms(std::span<T const> const in) -> T
{
    BOOST_ASSERT(in.size() > 0);

    T sum{};

    for (auto const x : in)
    {
        sum += x * x;
    }

    return std::sqrt(sum / in.size());
}

namespace simd
{

template <std::floating_point T>
auto
rms(std::span<T const> const in) -> T
{
    BOOST_ASSERT(in.size() > 0);
    BOOST_ASSERT(mipp::isAligned(in.data()));
    BOOST_ASSERT(in.size() % mipp::N<T>() == 0);

    mipp::Reg<T> sum(T{});

    constexpr auto const step = mipp::N<T>();
    for (auto i = in.data(), e = (i + in.size()); i != e; i += step)
    {
        mipp::Reg<T> reg_in = i;
        sum = mipp::fmadd(reg_in, reg_in, sum);
    }

    return std::sqrt(mipp::sum(sum) / in.size());
}

} // namespace simd

} // namespace piejam::audio::dsp
