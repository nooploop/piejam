// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/dsp/mipp_iterator.h>
#include <piejam/audio/sample_rate.h>

#include <piejam/functional/operators.h>
#include <piejam/math.h>
#include <piejam/numeric/pow_n.h>

#include <mipp.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <concepts>
#include <numeric>

namespace piejam::audio::dsp
{

template <std::floating_point T = float>
class rms_level_meter
{
    static constexpr std::chrono::milliseconds default_rms_measure_time{800};
    static constexpr float default_min_level = 0.001f; // -60 dB

public:
    using value_type = mipp::Reg<std::remove_const_t<T>>;

    explicit rms_level_meter(
            sample_rate sr,
            std::chrono::milliseconds rms_measure_time =
                    default_rms_measure_time,
            T min_level = default_min_level)
        : m_min_level{min_level}
        , m_sqr_history(
                  math::round_down_to_multiple(
                          static_cast<std::size_t>(
                                  std::chrono::duration_cast<
                                          std::chrono::duration<T>>(
                                          rms_measure_time)
                                          .count() *
                                  sr.as_float<T>()),
                          mipp::N<T>()),
                  0.f)
    {
    }

    void process(std::span<T const> samples)
    {
        auto [pre, main, post] = mipp_range_split(samples);

        process_span(pre);
        process_main_span(main);
        process_span(post);
    }

    [[nodiscard]]
    auto level() const noexcept -> T
    {
        return math::flush_to_zero_if(
                std::sqrt(std::max(m_sqr_sum, T{0}) / m_sqr_history_size),
                less(m_min_level));
    }

    void reset()
    {
        std::ranges::fill(m_sqr_history, T{0});
        m_sqr_sum = 0.f;
    }

private:
    template <class V, class SqrSumIterator, class SamplesIterator>
    static auto process_part(
            SqrSumIterator sqrsum_first,
            SqrSumIterator sqrsum_last,
            SamplesIterator samples_first,
            SamplesIterator samples_last)
    {
        auto sub =
                std::reduce(sqrsum_first, sqrsum_last, V(T{0}), std::plus<V>{});

        std::ranges::transform(
                samples_first,
                samples_last,
                sqrsum_first,
                numeric::pow_n<2>);

        auto add =
                std::reduce(sqrsum_first, sqrsum_last, V(T{0}), std::plus<V>{});

        return std::tuple{sub, add};
    }

    void process_span(std::span<T const> samples)
    {
        std::size_t const samples_size = samples.size();

        if (samples_size == 0)
        {
            return;
        }

        std::size_t const history_size = m_sqr_history.size();

        if (samples_size < history_size)
        {
            auto [lo, hi] = ring_buffer_split(samples_size);
            auto mid_it = std::next(samples.begin(), lo.size());

            {
                auto [sub, add] = process_part<T>(
                        lo.begin(),
                        lo.end(),
                        samples.begin(),
                        mid_it);

                adapt_sqr_sum(sub, add);
            }

            if (hi.size() > 0)
            {
                auto [sub, add] = process_part<T>(
                        hi.begin(),
                        hi.end(),
                        mid_it,
                        samples.end());

                adapt_sqr_sum(sub, add);
            }

            advance_position(samples_size);
        }
        else
        {
            std::ranges::transform(
                    std::next(samples.begin(), samples_size - history_size),
                    samples.end(),
                    m_sqr_history.begin(),
                    numeric::pow_n<2>);

            recompute_squared_sum();
        }
    }

    void process_main_span(std::span<T const> samples)
    {
        std::size_t samples_size = samples.size();

        if (m_position % mipp::N<T>() != 0) [[unlikely]]
        {
            process_span(samples);
            return;
        }

        auto samples_data = samples.data();
        std::size_t history_size = m_sqr_history.size();

        BOOST_ASSERT(samples_size % mipp::N<T>() == 0);
        BOOST_ASSERT(mipp::isAligned(samples_data));

        if (samples_size < history_size)
        {
            auto [lo, hi] = ring_buffer_split(samples_size);

            auto lo_mipp = mipp_range(lo);
            auto hi_mipp = mipp_range(hi);
            auto mid_it = mipp_iterator{samples_data + lo.size()};

            {
                auto [sub, add] = process_part<mipp::Reg<T>>(
                        lo_mipp.begin(),
                        lo_mipp.end(),
                        mipp_iterator{samples_data},
                        mid_it);

                adapt_sqr_sum(mipp::sum(sub), mipp::sum(add));
            }

            if (hi.size() > 0)
            {
                auto [sub, add] = process_part<mipp::Reg<T>>(
                        hi_mipp.begin(),
                        hi_mipp.end(),
                        mid_it,
                        mipp_iterator{samples_data + samples_size});

                adapt_sqr_sum(mipp::sum(sub), mipp::sum(add));
            }

            advance_position(samples_size);
        }
        else
        {
            std::ranges::transform(
                    mipp_iterator{samples_data + samples_size - history_size},
                    mipp_iterator{samples_data + samples_size},
                    mipp_iterator{m_sqr_history.data()},
                    numeric::pow_n<2>);

            recompute_squared_sum();
        }
    }

    void adapt_sqr_sum(T sub, T add)
    {
        m_sqr_sum = m_sqr_sum - sub + add;
    }

    void advance_position(std::size_t num_samples)
    {
        m_position += num_samples;

        std::size_t history_size = m_sqr_history.size();
        if (m_position >= history_size)
        {
            m_position -= history_size;
        }
    }

    void recompute_squared_sum()
    {
        auto mipprng = mipp_range(std::span{std::as_const(m_sqr_history)});
        m_sqr_sum = mipp::sum(std::reduce(
                mipprng.begin(),
                mipprng.end(),
                mipp::Reg<T>(T{0}),
                std::plus<mipp::Reg<T>>{}));
    }

    auto ring_buffer_split(std::size_t num_samples)
            -> std::tuple<std::span<T>, std::span<T>>
    {
        auto const history_data = m_sqr_history.data();
        auto const history_size = m_sqr_history.size();
        auto const first = history_data + m_position;

        if (m_position + num_samples < history_size)
        {
            auto last = first + num_samples;
            return std::tuple{std::span{first, last}, std::span{last, last}};
        }
        else
        {
            auto size = history_size - m_position;
            return std::tuple{
                    std::span{first, size},
                    std::span{history_data, num_samples - size}};
        }
    }

    T m_min_level;
    mipp::vector<T> m_sqr_history;

    std::size_t m_position{};
    T m_sqr_history_size{static_cast<T>(m_sqr_history.size())};
    T m_sqr_sum{};
};

} // namespace piejam::audio::dsp
