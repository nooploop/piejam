// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/box_fwd.h>
#include <piejam/scope_guard.h>

#include <boost/callable_traits/args.hpp>

#include <memory>
#include <utility>

namespace piejam
{

namespace detail
{

struct box_equal
{
    template <class T>
    constexpr auto operator()(
            std::shared_ptr<T const> const& l,
            std::shared_ptr<T const> const& r) const noexcept -> bool
    {
        return l.get() == r.get() || *l == *r;
    }
};

struct box_eq
{
    template <class T>
    constexpr auto operator()(
            std::shared_ptr<T const> const& l,
            std::shared_ptr<T const> const& r) const noexcept -> bool
    {
        return l.get() == r.get();
    }
};

template <class>
struct is_box : std::false_type
{
};

template <class T, class Eq>
struct is_box<box<T, Eq>> : std::true_type
{
};

template <class B>
constexpr bool is_box_v = is_box<B>::value;

template <class From, class To>
concept convertible_to_box_value =
        std::is_convertible_v<From, To> && !is_box_v<std::remove_cvref_t<From>>;

template <class T, class Eq>
class box
{
public:
    using value_type = T;

    box()
        : box(std::in_place, T{})
    {
    }

    template <convertible_to_box_value<T const> U>
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

    template <convertible_to_box_value<T const> U>
    auto operator=(U&& value) -> box&
    {
        m_value = std::make_shared<T const>(std::forward<U>(value));
        return *this;
    }

    template <std::invocable<T&> U>
    auto update(U&& u)
    {
        static_assert(std::is_same_v<
                      std::tuple_element_t<
                              0,
                              boost::callable_traits::args_t<U>>,
                      T&>);
        auto value = std::make_shared<T>(*m_value);
        on_scope_exit on_exit([this, value]() { m_value = std::move(value); });
        return u(*value);
    }

    auto operator==(box const& other) const noexcept -> bool
    {
        return Eq{}(m_value, other.m_value);
    }

    auto operator!=(box const& other) const noexcept -> bool
    {
        return !(*this == other);
    }

private:
    std::shared_ptr<T const> m_value;
};

template <class T, class Eq>
auto
operator==(box<T, Eq> const& lhs, T const& rhs) -> bool
{
    return lhs.get() == rhs;
}

template <class T, class Eq>
auto
operator==(T const& lhs, box<T, Eq> const& rhs) -> bool
{
    return lhs == rhs.get();
}

} // namespace detail

template <class T>
auto
box_(T&& t)
{
    return box<std::decay_t<T>>{std::forward<T>(t)};
};

template <class T>
auto
unique_box_(T&& t)
{
    return unique_box<std::decay_t<T>>{std::forward<T>(t)};
}

} // namespace piejam
