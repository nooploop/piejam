// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

class period_count
{
public:
    constexpr period_count() noexcept = default;
    explicit constexpr period_count(unsigned const value) noexcept
        : m_value(value)
    {
    }

    constexpr auto valid() const noexcept -> bool { return m_value != 0; }
    constexpr auto invalid() const noexcept -> bool { return m_value == 0; }

    constexpr auto get() const noexcept -> unsigned { return m_value; }

    constexpr auto operator==(period_count const&) const noexcept
            -> bool = default;

private:
    unsigned m_value{};
};

inline constexpr std::array preferred_period_counts{
        period_count(2u),
        period_count(3u),
        period_count(4u),
        period_count(5u),
        period_count(6u)};

using period_counts_t = boost::container::
        static_vector<period_count, preferred_period_counts.size()>;

} // namespace piejam::audio
