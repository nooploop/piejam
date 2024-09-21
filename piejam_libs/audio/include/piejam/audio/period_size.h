// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/container/static_vector.hpp>

#include <array>

namespace piejam::audio
{

class period_size
{
public:
    constexpr period_size() noexcept = default;

    explicit constexpr period_size(unsigned const value) noexcept
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

    constexpr auto operator==(period_size const&) const noexcept
            -> bool = default;

private:
    unsigned m_value{};
};

inline constexpr period_size max_period_size{1024u};
inline constexpr std::array preferred_period_sizes{
        period_size(16u),
        period_size(32u),
        period_size(64u),
        period_size(128u),
        period_size(256u),
        period_size(512u),
        max_period_size};

using period_sizes_t = boost::container::
        static_vector<period_size, preferred_period_sizes.size()>;

} // namespace piejam::audio
