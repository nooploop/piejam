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

#include <piejam/runtime/parameter/fwd.h>

#include <tuple>

namespace piejam::runtime::parameter
{

template <class... Parameter>
class maps_collection
{
public:
    template <class P>
    auto get_map() const -> map<P> const&
    {
        return std::get<map<P>>(m_maps);
    }

    template <class P>
    auto get_map() -> map<P>&
    {
        return std::get<map<P>>(m_maps);
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
    void remove(id_t<P> const id)
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
    auto get(id_t<P> const id) const
    {
        return get_map<P>().get(id);
    }

    template <class P, class V>
    auto set(id_t<P> const id, V&& value) -> bool
    {
        return get_map<P>().set(id, std::forward<V>(value));
    }

private:
    std::tuple<map<Parameter>...> m_maps;
};

} // namespace piejam::runtime::parameter
