// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <boost/assert.hpp>

#include <algorithm>
#include <concepts>
#include <iterator>

namespace piejam::audio
{

template <std::floating_point T = float>
class smoother
{
public:
    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = int;

        constexpr iterator() noexcept = default;

        constexpr explicit iterator(smoother<T>& smoother)
            : m_smoother(std::addressof(smoother))
        {
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> T
        {
            BOOST_ASSERT(m_smoother);

            return m_smoother->current();
        }

        constexpr auto operator++() noexcept -> iterator&
        {
            BOOST_ASSERT(m_smoother);

            m_smoother->advance();
            return *this;
        }

        constexpr auto operator++(int) noexcept -> iterator
        {
            BOOST_ASSERT(m_smoother);

            auto copy = *this;
            m_smoother->advance();
            return copy;
        }

        [[nodiscard]] constexpr auto
        operator==(iterator const& other) const noexcept -> bool
        {
            return this == &other;
        }

    private:
        smoother<T>* m_smoother{};
    };

    constexpr smoother() noexcept = default;

    constexpr explicit smoother(T const current) noexcept
        : m_current(current)
        , m_target(current)
    {
    }

    constexpr void set(T target, std::size_t steps_to_smooth) noexcept
    {
        if (m_target == target)
        {
            return;
        }

        m_target = target;
        m_steps_to_smooth = steps_to_smooth;

        if (m_steps_to_smooth == 0u)
        {
            m_current = m_target;
            m_inc = T{};
        }
        else
        {
            m_inc = (m_target - m_current) / m_steps_to_smooth;
        }
    }

    [[nodiscard]] constexpr auto generator() noexcept -> iterator
    {
        return iterator{*this};
    }

    [[nodiscard]] constexpr auto is_running() const noexcept -> bool
    {
        return m_steps_to_smooth != 0;
    }

    [[nodiscard]] constexpr auto current() const noexcept -> T
    {
        return m_current;
    }

private:
    constexpr void advance() noexcept
    {
        if (is_running())
        {
            m_current += m_inc;

            --m_steps_to_smooth;

            if (!is_running())
            {
                m_current = m_target;
            }
        }
    }

    std::size_t m_steps_to_smooth{};
    T m_inc{};
    T m_current{};
    T m_target{};
};

} // namespace piejam::audio
