// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/on_scope_exit.h>

#include <boost/callable_traits/args.hpp>

#include <memory>
#include <utility>

namespace piejam
{

template <class T>
class box;

namespace detail
{

template <class>
struct is_box : std::false_type
{
};

template <class T>
struct is_box<box<T>> : std::true_type
{
};

template <class B>
constexpr bool is_box_v = is_box<B>::value;

template <class From, class To>
concept convertible_to_box_value =
        std::is_convertible_v<From, To> && !is_box_v<std::remove_cvref_t<From>>;

} // namespace detail

template <class T>
class box
{
public:
    using value_type = T;

    class write_lock
    {
    public:
        explicit write_lock(std::shared_ptr<T const>& boxed_value)
            : m_boxed_value{boxed_value}
        {
        }

        ~write_lock()
        {
            m_boxed_value = m_value;
        }

        write_lock(write_lock const&) = delete;
        write_lock(write_lock&&) = delete;

        auto operator=(write_lock const&) = delete;
        auto operator=(write_lock&&) = delete;

        auto operator*() const noexcept -> T&
        {
            return *m_value;
        }

        auto operator->() const noexcept -> T*
        {
            return m_value.get();
        }

    private:
        std::shared_ptr<T const>& m_boxed_value;
        std::shared_ptr<T> m_value{std::make_shared<T>(*m_boxed_value)};
    };

    box() = default;

    template <detail::convertible_to_box_value<T const> U>
    explicit box(U&& v)
        : box(std::in_place, std::forward<U>(v))
    {
    }

    template <class... Args>
    box(std::in_place_t, Args&&... args)
        : m_value(std::make_shared<T const>(std::forward<Args>(args)...))
    {
    }

    box(box const&) = default;

    box(box&& other) noexcept
        : m_value{other.m_value}
    {
    }

    auto operator=(box const&) -> box& = default;

    auto operator=(box&& other) noexcept -> box&
    {
        m_value = other.m_value;
        return *this;
    }

    auto get() const noexcept -> T const&
    {
        return *m_value;
    }

    operator T const&() const noexcept
    {
        return *m_value;
    }

    auto operator*() const noexcept -> T const&
    {
        return *m_value;
    }

    auto operator->() const noexcept -> T const*
    {
        return m_value.get();
    }

    template <detail::convertible_to_box_value<T const> U>
    auto operator=(U&& value) -> box&
    {
        m_value = std::make_shared<T const>(std::forward<U>(value));
        return *this;
    }

    auto lock() -> write_lock
    {
        return write_lock{m_value};
    }

    auto operator==(box const& other) const noexcept -> bool
    {
        return m_value.get() == other.m_value.get();
    }

    auto operator!=(box const& other) const noexcept -> bool
    {
        return !(*this == other);
    }

private:
    static auto get_default() -> std::shared_ptr<T const>
    {
        static std::shared_ptr<T const> const s_default{
                std::make_shared<T>(T{})};
        return s_default;
    }

    std::shared_ptr<T const> m_value{get_default()};
};

template <class T>
auto
operator==(box<T> const& lhs, T const& rhs) -> bool
{
    return lhs.get() == rhs;
}

template <class T>
auto
operator==(T const& lhs, box<T> const& rhs) -> bool
{
    return lhs == rhs.get();
}

template <class T, class = std::enable_if_t<!detail::is_box_v<T>>>
box(T&& t) -> box<std::remove_cvref_t<T>>;

} // namespace piejam
