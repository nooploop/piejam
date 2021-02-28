// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pair.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameters.h>

namespace piejam::runtime
{

namespace detail
{

template <class P>
struct parameter_maps_access
{
    static auto empty(parameter_maps const&) noexcept -> bool;
    static auto size(parameter_maps const&) noexcept -> std::size_t;

    static auto add(parameter_maps&, P) -> parameter::id_t<P>;
    static auto add(parameter_maps&, P, parameter::value_type_t<P>)
            -> parameter::id_t<P>;

    static auto remove(parameter_maps&, parameter::id_t<P>) -> void;

    static auto contains(parameter_maps const&, parameter::id_t<P>) noexcept
            -> bool;

    static auto
    find_parameter(parameter_maps const&, parameter::id_t<P>) noexcept
            -> P const*;
    static auto
    get_parameter(parameter_maps const&, parameter::id_t<P>) noexcept
            -> P const&;

    static auto find(parameter_maps const&, parameter::id_t<P>) noexcept
            -> parameter::value_type_t<P> const*;
    static auto get(parameter_maps const&, parameter::id_t<P>) noexcept
            -> parameter::value_type_t<P> const&;

    static auto
    set(parameter_maps&, parameter::id_t<P>, parameter::value_type_t<P>)
            -> void;
    static auto set(parameter_maps&, parameter::id_value_map_t<P> const&)
            -> void;
};

extern template struct parameter_maps_access<bool_parameter>;
extern template struct parameter_maps_access<float_parameter>;
extern template struct parameter_maps_access<int_parameter>;
extern template struct parameter_maps_access<stereo_level_parameter>;

} // namespace detail

template <class P>
auto
empty(parameter_maps const& m) noexcept -> bool
{
    return detail::parameter_maps_access<P>::empty(m);
}

template <class P>
auto
size(parameter_maps const& m) noexcept -> std::size_t
{
    return detail::parameter_maps_access<P>::size(m);
}

template <class P>
auto
add_parameter(parameter_maps& m, P&& p) -> parameter::id_t<P>
{
    return detail::parameter_maps_access<P>::add(m, std::forward<P>(p));
}

template <class P, class V>
auto
add_parameter(parameter_maps& m, P&& p, V&& v) -> parameter::id_t<P>
{
    return detail::parameter_maps_access<P>::add(
            m,
            std::forward<P>(p),
            std::forward<V>(v));
}

template <class P>
auto
remove_parameter(parameter_maps& m, parameter::id_t<P> const id) -> void
{
    detail::parameter_maps_access<P>::remove(m, id);
}

template <class P>
auto
contains_parameter(parameter_maps const& m, parameter::id_t<P> const id) -> bool
{
    return detail::parameter_maps_access<P>::contains(m, id);
}

template <class P>
auto
find_parameter(parameter_maps const& m, parameter::id_t<P> const id) noexcept
        -> P const*
{
    return detail::parameter_maps_access<P>::find_parameter(m, id);
}

template <class P>
auto
get_parameter(parameter_maps const& m, parameter::id_t<P> const id) noexcept
        -> P const&
{
    return detail::parameter_maps_access<P>::get_parameter(m, id);
}

template <class P>
auto
find_parameter_value(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept
        -> parameter::value_type_t<P> const*
{
    return detail::parameter_maps_access<P>::find(m, id);
}

template <class P>
auto
get_parameter_value(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept
        -> parameter::value_type_t<P> const&
{
    return detail::parameter_maps_access<P>::get(m, id);
}

template <class P, class V>
auto
set_parameter_value(parameter_maps& m, parameter::id_t<P> const id, V&& v)
{
    return detail::parameter_maps_access<P>::set(m, id, std::forward<V>(v));
}

template <class P>
auto
set_parameter_values(parameter_maps& m, parameter::id_value_map_t<P> const& vs)
{
    return detail::parameter_maps_access<P>::set(m, vs);
}

} // namespace piejam::runtime
