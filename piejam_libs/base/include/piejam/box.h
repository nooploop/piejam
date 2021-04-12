// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/scope_guard.h>

#include <memory>
#include <utility>

namespace piejam
{

namespace detail
{

struct box_eq
{
    template <class T>
    constexpr bool operator()(
            std::shared_ptr<T const> const& l,
            std::shared_ptr<T const> const& r) const noexcept
    {
        return l.get() == r.get() || *l == *r;
    }
};

struct box_weak_eq
{
    template <class T>
    constexpr bool operator()(
            std::shared_ptr<T const> const& l,
            std::shared_ptr<T const> const& r) const noexcept
    {
        return l.get() == r.get();
    }
};

template <class T, class Eq>
class box;

template <class>
struct is_box : std::false_type
{
};

template <class U, class EqU>
struct is_box<box<U, EqU>> : std::true_type
{
};

template <class B>
constexpr bool is_box_v = is_box<B>::value;

template <class T, class Eq>
class box
{
public:
    box()
        : box(std::in_place, T{})
    {
    }

    template <std::convertible_to<T const> U>
    box(U&& v) requires(!is_box_v<std::decay_t<U>>)
        : box(std::in_place, std::forward<U>(v))
    {
    }

    template <class... Args>
    box(std::in_place_t, Args&&... args)
        : m_value(std::make_shared<T const>(std::forward<Args>(args)...))
    {
    }

    auto get() const noexcept -> T const& { return *m_value; }

    operator T const &() const noexcept { return *m_value; }

    auto operator*() const noexcept -> T const& { return *m_value; }
    auto operator->() const noexcept -> T const* { return m_value.get(); }

    template <std::convertible_to<T const> U>
    auto operator=(U&& value) -> box& requires(!is_box_v<std::decay_t<U>>)
    {
        m_value = std::make_shared<T const>(std::forward<U>(value));
        return *this;
    }

    template <std::invocable<T&> U>
    auto update(U&& u)
    {
        auto value = std::make_shared<T>(*m_value);
        on_scope_exit on_exit([this, value]() { m_value = std::move(value); });
        return u(*value);
    }

    bool operator==(box const& other) const noexcept
    {
        return Eq{}(m_value, other.m_value);
    }

    bool operator!=(box const& other) const noexcept
    {
        return !(*this == other);
    }

private:
    std::shared_ptr<T const> m_value;
};

} // namespace detail

template <class T>
using box = detail::box<T, detail::box_eq>;

template <class T>
using unique_box = detail::box<T, detail::box_weak_eq>;

} // namespace piejam
