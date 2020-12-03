// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <cmath>
#include <cstddef>

namespace piejam::audio
{

class level_peak_meter
{
public:
    using value_type = float;

    level_peak_meter(std::size_t decay_time)
        : m_decay_factor(1.f - 1.f / decay_time)
    {
    }

    void push_back(float const x)
    {
        auto const abs_x = std::abs(x);
        auto const decayed = m_peak_level * m_decay_factor;
        m_peak_level = abs_x > decayed ? abs_x : decayed;
    }

    auto get() const noexcept -> float { return m_peak_level; }

    void clear() noexcept { m_peak_level = 0.f; }

private:
    float m_peak_level{};
    float m_decay_factor{.5f};
};

} // namespace piejam::audio
