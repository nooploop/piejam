// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>

#include <piejam/numeric/pow_n.h>

#include <boost/hof/compose.hpp>

#include <numeric>
#include <span>

namespace piejam::audio::dsp
{

template <std::floating_point T>
class pitch_yin
{
public:
    explicit pitch_yin(sample_rate const sr)
        : m_sr{sr}
    {
    }

    static constexpr T const threshold{0.1f};

    auto run(std::span<T const> const in) -> T
    {
        std::size_t const e = in.size() / 2;
        T a = T{1};
        T b = T{1};
        T c{};
        T cumulative_sum{};
        for (std::size_t tau = 1; tau < e; ++tau)
        {
            T const sum = sqr_difference_sum(in, e, tau);
            cumulative_sum += sum;
            c = tau * sum / cumulative_sum;

            if (b < threshold && b < c)
            {
                // quadratic interpolation
                T const betterTau =
                        (tau - 1) + (a - c) / (T{2} * (a - T{2} * b + c));
                return m_sr.as_float<T>() / betterTau;
            }

            a = b;
            b = c;
        }

        return T{};
    }

private:
    static constexpr auto sqr_difference_sum(
            std::span<T const> const in,
            std::size_t const e,
            std::size_t const tau) -> T
    {
        return std::transform_reduce(
                in.begin(),
                std::next(in.begin(), e),
                std::next(in.begin(), tau),
                T{},
                std::plus<>{},
                boost::hof::compose(numeric::pow_n<2>, std::minus<>{}));
    }

    sample_rate m_sr;
};

} // namespace piejam::audio::dsp
