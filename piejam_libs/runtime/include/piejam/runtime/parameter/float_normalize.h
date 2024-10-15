// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/float_descriptor.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>

#include <algorithm>
#include <cmath>

namespace piejam::runtime::parameter
{

namespace detail
{

[[nodiscard]]
constexpr auto
to_normalized(float const value, float const min, float const max) -> float
{
    return (value - min) / (max - min);
}

[[nodiscard]]
constexpr auto
from_normalized(float const norm_value, float const min, float const max)
        -> float
{
    return norm_value * (max - min) + min;
}

} // namespace detail

[[nodiscard]]
constexpr auto
to_normalized_linear(float_descriptor const& p, float const value) -> float
{
    return detail::to_normalized(value, p.min, p.max);
}

template <float Min, float Max>
[[nodiscard]]
constexpr auto
to_normalized_linear_static(float_descriptor const&, float const value) -> float
{
    return detail::to_normalized(value, Min, Max);
}

[[nodiscard]]
constexpr auto
from_normalized_linear(float_descriptor const& p, float const norm_value)
        -> float
{
    return detail::from_normalized(norm_value, p.min, p.max);
}

template <float Min, float Max>
[[nodiscard]]
constexpr auto
from_normalized_linear_static(float_descriptor const&, float const norm_value)
        -> float
{
    return detail::from_normalized(norm_value, Min, Max);
}

[[nodiscard]]
constexpr auto
to_normalized_log(float_descriptor const& p, float const value) -> float
{
    return detail::to_normalized(
            std::log(value),
            std::log(p.min),
            std::log(p.max));
}

[[nodiscard]]
constexpr auto
from_normalized_log(float_descriptor const& p, float const norm_value) -> float
{
    return std::exp(detail::from_normalized(
            norm_value,
            std::log(p.min),
            std::log(p.max)));
}

template <float Min_dB, float Max_dB>
[[nodiscard]]
constexpr auto
to_normalized_dB(float_descriptor const&, float const value) -> float
{
    return detail::to_normalized(std::log10(value) * 20.f, Min_dB, Max_dB);
}

template <float Min_dB, float Max_dB>
[[nodiscard]]
constexpr auto
from_normalized_dB(float_descriptor const&, float const norm_value) -> float
{
    return std::pow(
            10.f,
            detail::from_normalized(norm_value, Min_dB, Max_dB) / 20.f);
}

struct normalized_dB_mapping
{
    float normalized{};
    float dB{};
};

template <auto Mapping, float Min_dB>
constexpr auto
to_normalized_dB_mapping(float const value)
{
    float const value_dB = math::to_dB(value);
    if (value_dB <= Min_dB)
    {
        return 0.f;
    }

    auto first = std::ranges::begin(Mapping);
    if (value_dB < first->dB)
    {
        return math::linear_map(
                value_dB,
                Min_dB,
                first->dB,
                0.f,
                first->normalized);
    }

    auto lower = std::ranges::adjacent_find(
            Mapping,
            in_closed(value_dB),
            &normalized_dB_mapping::dB);
    BOOST_ASSERT(lower != std::ranges::end(Mapping));
    auto const upper = std::next(lower);

    return math::linear_map(
            value_dB,
            lower->dB,
            upper->dB,
            lower->normalized,
            upper->normalized);
}

template <auto Mapping, float Min_dB>
constexpr auto
from_normalized_dB_maping(float const norm_value)
{
    if (norm_value == 0.f)
    {
        return 0.f;
    }

    auto first = std::ranges::begin(Mapping);
    if (norm_value < first->normalized)
    {
        return math::from_dB(math::linear_map(
                norm_value,
                0.f,
                first->normalized,
                Min_dB,
                first->dB));
    }

    auto lower = std::ranges::adjacent_find(
            Mapping,
            in_closed(norm_value),
            &normalized_dB_mapping::normalized);
    BOOST_ASSERT(lower != std::ranges::end(Mapping));
    auto const upper = std::next(lower);

    return math::from_dB(math::linear_map(
            norm_value,
            lower->normalized,
            upper->normalized,
            lower->dB,
            upper->dB));
}

} // namespace piejam::runtime::parameter
