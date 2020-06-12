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

#include <algorithm>

namespace piejam::audio
{

template <class T = float>
class smoother
{
    static_assert(std::is_scalar_v<T>);

public:
    constexpr void set(T target, std::size_t framesToSmooth) noexcept
    {
        if (target != m_target)
        {
            m_target = target;
            m_framesToSmooth = framesToSmooth;

            if (!m_framesToSmooth)
            {
                m_current = m_target;
                m_inc = T{};
            }
            else
            {
                m_inc = (m_target - m_current) / m_framesToSmooth;
            }
        }
    }

    constexpr auto operator*() const noexcept -> T { return m_current; }

    constexpr auto operator++() noexcept -> smoother&
    {
        advance(1);
        return *this;
    }

    constexpr auto operator++(int) noexcept -> smoother
    {
        auto copy = *this;
        advance(1);
        return copy;
    }

    constexpr void advance(std::size_t const frames) noexcept
    {
        std::size_t const framesToProcess = std::min(frames, m_framesToSmooth);
        if (framesToProcess)
        {
            m_current += m_inc * framesToProcess;
            m_framesToSmooth -= framesToProcess;

            if (!m_framesToSmooth)
            {
                m_current = m_target;
            }
        }
    }

    constexpr bool is_running() const noexcept { return m_framesToSmooth != 0; }

    constexpr auto current() const noexcept -> T { return m_current; }

private:
    std::size_t m_framesToSmooth{};
    T m_inc{};
    T m_current{};
    T m_target{};
};

} // namespace piejam::audio
