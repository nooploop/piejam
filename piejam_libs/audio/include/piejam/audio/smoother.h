// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <iterator>

namespace piejam::audio
{

template <class T = float>
class smoother
{
    static_assert(std::is_floating_point_v<T>);

public:
    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = int;

        constexpr iterator() noexcept = default;
        constexpr iterator(smoother<T>& smoother)
            : m_smoother(std::addressof(smoother))
        {
        }

        constexpr auto operator*() const noexcept -> T
        {
            return m_smoother->current();
        }

        constexpr auto operator++() noexcept -> iterator&
        {
            m_smoother->advance();
            return *this;
        }

        constexpr auto operator++(int) noexcept -> iterator
        {
            auto copy = *this;
            m_smoother->advance();
            return copy;
        }

        constexpr bool operator==(iterator const& other) const noexcept
        {
            return this == &other;
        }

    private:
        smoother<T>* m_smoother{};
    };

    constexpr smoother() noexcept = default;
    constexpr smoother(T const current) noexcept
        : m_current(current)
        , m_target(current)
    {
    }

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

    constexpr void advance() noexcept
    {
        if (m_frames_to_smooth)
        {
            m_current += m_inc;
            --m_frames_to_smooth;

            if (!m_frames_to_smooth)
                m_current = m_target;
        }
    }

    constexpr auto input_iterator() noexcept -> iterator { return {*this}; }

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
