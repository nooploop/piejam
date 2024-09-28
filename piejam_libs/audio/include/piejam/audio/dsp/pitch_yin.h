// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/sample_rate.h>

#include <piejam/numeric/pow_n.h>

#include <mipp.h>

#include <boost/hof/compose.hpp>

#include <concepts>
#include <numeric>
#include <span>

namespace piejam::audio::dsp
{

namespace detail
{

template <std::floating_point T>
[[nodiscard]]
static constexpr auto
sqr_difference_sum(
        std::span<T const> const in,
        std::size_t const e,
        std::size_t const tau) -> T
{
    constexpr auto const N = mipp::N<T>();
    auto const data = in.data();
    auto const offset = tau % N;
    if (offset == 0)
    {
        return mipp::sum(std::transform_reduce(
                mipp_iterator{data},
                mipp_iterator{data + e},
                mipp_iterator{data + tau},
                mipp::Reg<T>(T{}),
                std::plus<>{},
                boost::hof::compose(numeric::pow_n<2>, std::minus<>{})));
    }
    else
    {
        mipp::Reg<T> sums(T{});

        auto mask = []<std::size_t... I>(
                            std::index_sequence<I...>,
                            std::size_t offset) {
            return mipp::Msk<N>{I < (N - offset)...};
        }(std::make_index_sequence<N>{}, offset);

        mipp_iterator it_tau{data + tau - offset};
        mipp::Reg<T> reg_lo = mipp_lrot_n(*it_tau, offset);

        for (auto reg_i : std::ranges::subrange(
                     mipp_iterator{data},
                     mipp_iterator{data + e}))
        {
            ++it_tau;
            mipp::Reg<T> reg_hi = mipp_lrot_n(*it_tau, offset);

            sums = mipp_fsqradd(
                    reg_i - mipp::select(mask, reg_lo, reg_hi),
                    sums);

            reg_lo = reg_hi;
        }

        return mipp::sum(sums);
    }
}

} // namespace detail

template <std::floating_point T>
[[nodiscard]]
auto
pitch_yin(std::span<T const> const in, sample_rate const sr) -> T
{
    constexpr T const threshold{0.1f};

    std::size_t const e = in.size() / 2;
    T a = T{1};
    T b = T{1};
    T c{};
    T cumulative_sum{};
    for (std::size_t tau = 1; tau < e; ++tau)
    {
        T const sum = detail::sqr_difference_sum(in, e, tau);

        cumulative_sum += sum;
        c = tau * sum / cumulative_sum;

        if (b < threshold && b < c)
        {
            // quadratic interpolation
            T const betterTau =
                    (tau - 1) + (a - c) / (T{2} * (a - T{2} * b + c));
            return sr.as_float<T>() / betterTau;
        }

        a = b;
        b = c;
    }

    return T{};
}

} // namespace piejam::audio::dsp
