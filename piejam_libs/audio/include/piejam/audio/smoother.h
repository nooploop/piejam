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

        constexpr iterator(smoother<T>& smoother)
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
                m_advance = &smoother<T>::advance_still;
            }
            else
            {
                m_inc = (m_target - m_current) / m_frames_to_smooth;
                m_advance = &smoother<T>::advance_running;
            }
        }
    }

    [[nodiscard]] constexpr auto generator() noexcept -> iterator
    {
        return {*this};
    }

    [[nodiscard]] constexpr auto is_running() const noexcept -> bool
    {
        return m_frames_to_smooth != 0;
    }

    [[nodiscard]] constexpr auto current() const noexcept -> T
    {
        return m_current;
    }

private:
    constexpr void advance() noexcept
    {
        (this->*m_advance)();
    }

    constexpr void advance_running() noexcept
    {
        BOOST_ASSERT(is_running());

        m_current += m_inc;

        --m_frames_to_smooth;

        if (!is_running())
        {
            m_current = m_target;
            m_advance = &smoother<T>::advance_still;
        }
    }

    constexpr void advance_still() noexcept
    {
        BOOST_ASSERT(!is_running());
    }

    std::size_t m_frames_to_smooth{};
    T m_inc{};
    T m_current{};
    T m_target{};
    using advance_t = void (smoother<T>::*)();
    advance_t m_advance{&smoother<T>::advance_still};
};

} // namespace piejam::audio
