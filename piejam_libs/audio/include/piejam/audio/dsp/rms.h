// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/mipp_iterator.h>

#include <piejam/numeric/pow_n.h>

#include <mipp.h>

#include <boost/assert.hpp>

#include <cmath>
#include <concepts>
#include <numeric>
#include <span>

namespace piejam::audio::dsp
{

template <std::floating_point T>
[[nodiscard]]
auto
rms(std::span<T const> const in) -> T
{
    BOOST_ASSERT(in.size() > 0);
    return std::sqrt(
            std::transform_reduce(
                    in.begin(),
                    in.end(),
                    T{},
                    std::plus<>{},
                    numeric::pow_n<2>) /
            in.size());
}

namespace simd
{

template <std::floating_point T>
[[nodiscard]]
auto
rms(std::span<T const> const in) -> T
{
    auto rng = mipp_range(in);
    auto sums = std::transform_reduce(
            rng.begin(),
            rng.end(),
            mipp::Reg<T>(T{}),
            std::plus<>{},
            numeric::pow_n<2>);

    return std::sqrt(mipp::sum(sums) / in.size());
}

} // namespace simd

} // namespace piejam::audio::dsp
