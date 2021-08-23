// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/container/static_vector.hpp>

#include <array>

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

    constexpr auto valid() const noexcept -> bool { return m_value != 0; }
    constexpr auto invalid() const noexcept -> bool { return m_value == 0; }

    constexpr auto get() const noexcept -> unsigned { return m_value; }

    constexpr auto as_int() const noexcept -> int
    {
        return static_cast<int>(m_value);
    }

    constexpr auto as_float() const noexcept -> float
    {
        return static_cast<float>(m_value);
    }

    constexpr auto operator==(sample_rate const&) const noexcept
            -> bool = default;

private:
    unsigned m_value{};
};

inline constexpr std::array preferred_sample_rates{
        sample_rate(11025u),
        sample_rate(12000u),
        sample_rate(22050u),
        sample_rate(24000u),
        sample_rate(44100u),
        sample_rate(48000u),
        sample_rate(88200u),
        sample_rate(96000u),
        sample_rate(176400u),
        sample_rate(192000u)};

using sample_rates_t = boost::container::
        static_vector<sample_rate, preferred_sample_rates.size()>;

} // namespace piejam::audio
