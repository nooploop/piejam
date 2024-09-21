// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/container/static_vector.hpp>

#include <array>
#include <chrono>

namespace piejam::audio
{

class sample_rate
{
public:
    constexpr sample_rate() noexcept = default;
    explicit constexpr sample_rate(unsigned const value) noexcept
        : m_value(value)
    {
    }

    [[nodiscard]] constexpr auto valid() const noexcept -> bool
    {
        return m_value != 0;
    }
    [[nodiscard]] constexpr auto invalid() const noexcept -> bool
    {
        return m_value == 0;
    }

    [[nodiscard]] constexpr auto value() const noexcept -> unsigned
    {
        return m_value;
    }

    [[nodiscard]] constexpr auto as_int() const noexcept -> int
    {
        return static_cast<int>(m_value);
    }

    [[nodiscard]] constexpr auto as_float() const noexcept -> float
    {
        return static_cast<float>(m_value);
    }

    template <class Rep = float>
    constexpr auto to_nanoseconds(std::size_t const samples) const noexcept
            -> std::chrono::duration<Rep, std::nano>
    {
        return std::chrono::duration<Rep, std::nano>(
                (static_cast<Rep>(samples) / static_cast<Rep>(m_value)) *
                static_cast<Rep>(std::nano::den));
    }

    template <class Rep, class Period>
    constexpr auto
    to_samples(std::chrono::duration<Rep, Period> const& dur) const noexcept
            -> std::size_t
    {
        return static_cast<std::size_t>(
                m_value *
                (dur / std::chrono::duration<double, std::ratio<1>>(1)));
    }

    constexpr auto operator==(sample_rate const&) const noexcept
            -> bool = default;

private:
    unsigned m_value{};
};

inline constexpr std::array preferred_sample_rates{
        sample_rate(44100u),
        sample_rate(48000u),
        sample_rate(88200u),
        sample_rate(96000u),
        sample_rate(176400u),
        sample_rate(192000u)};

using sample_rates_t = boost::container::
        static_vector<sample_rate, preferred_sample_rates.size()>;

} // namespace piejam::audio
