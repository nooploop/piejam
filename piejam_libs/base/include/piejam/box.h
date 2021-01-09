// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/scope_guard.h>

#include <concepts>
#include <memory>

namespace piejam
{

template <class T>
class box
{
public:
    template <class... Args>
    box(Args&&... args)
        : m_value(std::make_shared<T const>(std::forward<Args>(args)...))
    {
    }

    auto get() const noexcept -> T const& { return *m_value; }

    operator T const &() const noexcept { return *m_value; }

    auto operator*() const noexcept -> T const& { return *m_value; }
    auto operator->() const noexcept -> T const* { return m_value.get(); }

    template <std::convertible_to<T> U>
    auto operator=(U&& value) -> box<T>&
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

    bool operator==(box const& r) const noexcept
            requires std::equality_comparable<T>
    {
        return m_value.get() == r.m_value.get() || get() == r.get();
    }

    bool operator==(box const& r) const noexcept
            requires(!std::equality_comparable<T>)
    {
        return m_value.get() == r.m_value.get();
    }

    bool operator!=(box const& r) const noexcept
            requires std::equality_comparable<T>
    {
        return m_value.get() != r.m_value.get() && get() != r.get();
    }

    bool operator!=(box const& r) const noexcept
            requires(!std::equality_comparable<T>)
    {
        return m_value.get() != r.m_value.get();
    }

    bool eq(box const& other) const noexcept
    {
        return m_value.get() == other.m_value.get();
    }

    bool neq(box const& other) const noexcept
    {
        return m_value.get() == other.m_value.get();
    }

private:
    std::shared_ptr<T const> m_value;
};

} // namespace piejam
