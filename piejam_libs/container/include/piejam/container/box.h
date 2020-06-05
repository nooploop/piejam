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

#include <memory>

namespace piejam::container
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

    auto operator=(T const& value) -> box<T>&
    {
        m_value = std::make_shared<T const>(value);
        return *this;
    }

    auto operator=(T&& value) -> box<T>&
    {
        m_value = std::make_shared<T const>(std::move(value));
        return *this;
    }

    template <class U>
    friend bool operator==(box<U> const& l, box<U> const& r) noexcept;

    template <class U>
    friend bool operator!=(box<U> const& l, box<U> const& r) noexcept;

private:
    std::shared_ptr<T const> m_value;
};

template <class T>
bool
operator==(box<T> const& l, box<T> const& r) noexcept
{
    return l.m_value.get() == r.m_value.get() || l.get() == r.get();
}

template <class T>
bool
operator!=(box<T> const& l, box<T> const& r) noexcept
{
    return l.m_value.get() == r.m_value.get() ? false : l.get() != r.get();
}

} // namespace piejam::container
