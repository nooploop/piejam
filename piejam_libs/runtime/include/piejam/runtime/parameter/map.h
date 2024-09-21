// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

#include <boost/container/flat_map.hpp>

#include <memory>
#include <typeindex>

namespace piejam::runtime::parameter
{

template <template <class Parameter> class Slot>
class map
{
public:
    template <class P>
    using map_t = boost::container::flat_map<id_t<P>, Slot<P>>;

    template <class P>
    auto get_map() const -> map_t<P> const&
    {
        return *static_cast<map_t<P> const*>(m_maps.at(typeid(P)).get());
    }

    template <class P>
    auto get_map() -> map_t<P>&
    {
        auto it = m_maps.find(typeid(P));

        if (it == m_maps.end()) [[unlikely]]
        {
            it = m_maps.emplace(
                               std::type_index{typeid(P)},
                               std::make_shared<map_t<P>>())
                         .first;
        }

        return *static_cast<map_t<P>*>(it->second.get());
    }

    template <class P, class... Args>
    auto emplace(id_t<P> const id, Args&&... args)
    {
        return BOOST_VERIFY(
                get_map<P>()
                        .emplace(
                                std::piecewise_construct,
                                std::forward_as_tuple(id),
                                std::forward_as_tuple(
                                        std::forward<Args>(args)...))
                        .second);
    }

    template <class P>
    auto remove(id_t<P> const id) -> void
    {
        get_map<P>().erase(id);
    }

    template <class P>
    auto contains(id_t<P> const id) const noexcept -> bool
    {
        return get_map<P>().contains(id);
    }

    template <class P>
    auto find(id_t<P> const id) const noexcept -> Slot<P> const*
    {
        auto const& m = get_map<P>();
        auto it = m.find(id);
        return it != m.end() ? &it->second : nullptr;
    }

    template <class P>
    auto find(id_t<P> const id) noexcept -> Slot<P>*
    {
        auto& m = get_map<P>();
        auto it = m.find(id);
        return it != m.end() ? &it->second : nullptr;
    }

    template <class P>
    auto operator[](id_t<P> const id) const noexcept -> Slot<P> const&
    {
        auto const& m = get_map<P>();
        auto it = m.find(id);
        BOOST_ASSERT(it != m.end());
        return it->second;
    }

    template <class P>
    auto operator[](id_t<P> const id) noexcept -> Slot<P>&
    {
        auto& m = get_map<P>();
        auto it = m.find(id);
        BOOST_ASSERT(it != m.end());
        return it->second;
    }

private:
    boost::container::flat_map<std::type_index, std::shared_ptr<void>> m_maps;
};

} // namespace piejam::runtime::parameter
