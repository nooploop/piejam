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

#include <piejam/entity_map.h>

#include <boost/container/flat_map.hpp>

#include <concepts>

namespace piejam::runtime::parameter
{

template <class Parameter>
class map
{
public:
    using id_t = typename entity_map<Parameter>::id_t;
    using value_type = typename Parameter::value_type;

    template <std::same_as<Parameter> P>
    auto add(P&& p) -> id_t
    {
        auto value = p.default_value;
        auto id = m_parameters.add(std::forward<P>(p));
        m_values.emplace(id, std::move(value));
        return id;
    }

    template <std::same_as<Parameter> P, std::convertible_to<value_type> V>
    auto add(P&& p, V&& value) -> id_t
    {
        auto id = m_parameters.add(std::forward<P>(p));
        m_values.emplace(id, std::forward<V>(value));
        return id;
    }

    void remove(id_t id)
    {
        m_parameters.remove(id);
        m_values.erase(id);
    }

    auto contains(id_t id) const noexcept -> bool
    {
        return m_parameters.contains(id);
    }

    auto get(id_t id) const noexcept -> value_type const*
    {
        auto it = m_values.find(id);
        return it != m_values.end() ? std::addressof(it->second) : nullptr;
    }

    template <std::convertible_to<value_type> V>
    auto set(id_t id, V&& value) -> bool
    {
        auto it = m_values.find(id);
        if (it != m_values.end())
        {
            it->second = std::forward<V>(value);
            return true;
        }

        return false;
    }

private:
    entity_map<Parameter> m_parameters;
    boost::container::flat_map<id_t, value_type> m_values;
};

} // namespace piejam::runtime
