// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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

    bool empty() const noexcept { return m_parameters.empty(); }
    auto size() const noexcept -> std::size_t { return m_parameters.size(); }

    template <std::same_as<Parameter> P>
    auto add(P&& p) -> id_t
    {
        return add(std::forward<P>(p), p.default_value);
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

    auto get_parameter(id_t id) const noexcept -> Parameter const*
    {
        return m_parameters[id];
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

} // namespace piejam::runtime::parameter
