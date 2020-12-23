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
