// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>
#include <piejam/runtime/parameter/map.h>

#include <boost/container/flat_map.hpp>

#include <memory>
#include <typeindex>

namespace piejam::runtime::parameter
{

class maps_collection
{
public:
    template <class P>
    auto get_map() const -> map<P> const&
    {
        return *static_cast<map<P> const*>(m_maps.at(typeid(P)).get());
    }

    template <class P>
    auto get_map() -> map<P>&
    {
        auto it = m_maps.find(typeid(P));

        if (it == m_maps.end()) [[unlikely]]
        {
            it = m_maps.emplace(
                               std::type_index{typeid(P)},
                               std::make_shared<map<P>>())
                         .first;
        }

        return *static_cast<map<P>*>(it->second.get());
    }

    template <class P>
    auto add(P&& p) -> id_t<P>
    {
        return get_map<P>().add(std::forward<P>(p));
    }

    template <class P, class V>
    auto add(P&& p, V&& value) -> id_t<P>
    {
        return get_map<P>().add(std::forward<P>(p), std::forward<V>(value));
    }

    template <class P>
    auto remove(id_t<P> const id) -> void
    {
        get_map<P>().remove(id);
    }

    template <class P>
    auto contains(id_t<P> const id) const noexcept -> bool
    {
        return get_map<P>().contains(id);
    }

    template <class P>
    auto get_parameter(id_t<P> const id) const noexcept -> P const*
    {
        return get_map<P>().get_parameter(id);
    }

    template <class P>
    auto find(id_t<P> const id) const noexcept -> value_type_t<P> const*
    {
        return get_map<P>().find(id);
    }

    template <class P>
    auto get(id_t<P> const id) const noexcept -> value_type_t<P> const&
    {
        return get_map<P>().get(id);
    }

    template <class P>
    auto get_cached(id_t<P> const id) const noexcept
            -> std::shared_ptr<value_type_t<P> const>
    {
        return get_map<P>().get_cached(id);
    }

    template <class P, class V>
    auto set(id_t<P> const id, V&& value) -> void
    {
        get_map<P>().set(id, std::forward<V>(value));
    }

    template <class P>
    auto set(id_value_map_t<P> const& id_values)
    {
        get_map<P>().set(id_values);
    }

private:
    boost::container::flat_map<std::type_index, std::shared_ptr<void>> m_maps;
};

} // namespace piejam::runtime::parameter
