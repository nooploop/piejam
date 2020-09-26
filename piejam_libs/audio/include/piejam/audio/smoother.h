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
    constexpr void set(T target, std::size_t frames_to_smooth) noexcept
    {
        if (target != m_target)
        {
            m_target = target;
            m_frames_to_smooth = frames_to_smooth;

            if (!m_frames_to_smooth)
            {
                m_current = m_target;
                m_inc = T{};
            }
            else
            {
                m_inc = (m_target - m_current) / m_frames_to_smooth;
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
        std::size_t const frames_to_process =
                std::min(frames, m_frames_to_smooth);
        if (frames_to_process)
        {
            m_current += m_inc * frames_to_process;
            m_frames_to_smooth -= frames_to_process;

            if (!m_frames_to_smooth)
            {
                m_current = m_target;
            }
        }
    }

    constexpr bool is_running() const noexcept
    {
        return m_frames_to_smooth != 0;
    }

    constexpr auto current() const noexcept -> T { return m_current; }

private:
    std::size_t m_frames_to_smooth{};
    T m_inc{};
    T m_current{};
    T m_target{};
};

} // namespace piejam::audio
