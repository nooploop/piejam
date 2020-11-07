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

#include <string>
#include <string_view>
#include <typeindex>
#include <utility>

namespace piejam::audio::engine
{

class event_port
{
public:
    template <class T>
    event_port(std::in_place_type_t<T>, std::string_view const& name = {})
        : m_type(typeid(T))
        , m_name(name)
    {
    }

    auto type() const noexcept -> std::type_index const& { return m_type; }
    auto name() const noexcept -> std::string const& { return m_name; }

private:
    std::type_index m_type;
    std::string m_name;
};

} // namespace piejam::audio::engine
