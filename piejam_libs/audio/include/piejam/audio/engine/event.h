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

#include <boost/intrusive/set_hook.hpp>

namespace piejam::audio::engine
{

template <class T>
class event final : public boost::intrusive::set_base_hook<>
{
    static_assert(std::is_trivially_destructible_v<T>);

public:
    event() = default;
    event(std::size_t const offset, T const& value)
        : m_offset(offset)
        , m_value(value)
    {
    }
    event(std::size_t const offset, T&& value)
        : m_offset(offset)
        , m_value(std::move(value))
    {
    }

    auto offset() const noexcept -> std::size_t { return m_offset; }
    auto value() const noexcept -> T const& { return m_value; }

private:
    std::size_t m_offset{};
    T m_value{};
};

} // namespace piejam::audio::engine
