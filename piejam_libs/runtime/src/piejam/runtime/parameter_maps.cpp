// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/parameter_maps.h>

#include <piejam/audio/pair.h>
#include <piejam/entity_id.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter/maps_collection.h>
#include <piejam/runtime/parameter_maps_access.h>

namespace piejam::runtime
{

struct parameter_maps::impl
{
    using maps_t = parameter::maps_collection<
            float_parameter,
            bool_parameter,
            int_parameter,
            stereo_level_parameter>;

    maps_t maps;
};

parameter_maps::parameter_maps()
    : m_impl(std::make_unique<impl>())
{
}

parameter_maps::parameter_maps(parameter_maps const& other)
    : m_impl(std::make_unique<impl>(other.m_impl->maps))
{
}

parameter_maps::~parameter_maps() = default;

auto
parameter_maps::operator=(parameter_maps const& other) -> parameter_maps&
{
    m_impl->maps = other.m_impl->maps;
    return *this;
}

namespace detail
{

template <class P>
auto
parameter_maps_access<P>::empty(parameter_maps const& m) noexcept -> bool
{
    return m.m_impl->maps.get_map<P>().empty();
}

template <class P>
auto
parameter_maps_access<P>::size(parameter_maps const& m) noexcept -> std::size_t
{
    return m.m_impl->maps.get_map<P>().size();
}

template <class P>
auto
parameter_maps_access<P>::add(parameter_maps& m, P p) -> parameter::id_t<P>
{
    return m.m_impl->maps.add(std::move(p));
}

template <class P>
auto
parameter_maps_access<P>::add(
        parameter_maps& m,
        P p,
        parameter::value_type_t<P> v) -> parameter::id_t<P>
{
    return m.m_impl->maps.add(std::move(p), std::move(v));
}

template <class P>
auto
parameter_maps_access<P>::remove(parameter_maps& m, parameter::id_t<P> const id)
        -> void
{
    m.m_impl->maps.remove(id);
}

template <class P>
auto
parameter_maps_access<P>::contains(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept -> bool
{
    return m.m_impl->maps.contains(id);
}

template <class P>
auto
parameter_maps_access<P>::find_parameter(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept -> P const*
{
    return m.m_impl->maps.get_parameter(id);
}

template <class P>
auto
parameter_maps_access<P>::get_parameter(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept -> P const&
{
    auto const* const p = m.m_impl->maps.get_parameter(id);
    BOOST_ASSERT(p);
    return *p;
}

template <class P>
auto
parameter_maps_access<P>::find(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept
        -> parameter::value_type_t<P> const*
{
    return m.m_impl->maps.find(id);
}

template <class P>
auto
parameter_maps_access<P>::get(
        parameter_maps const& m,
        parameter::id_t<P> const id) noexcept
        -> parameter::value_type_t<P> const&
{
    return m.m_impl->maps.get(id);
}

template <class P>
auto
parameter_maps_access<P>::set(
        parameter_maps& m,
        parameter::id_t<P> const id,
        parameter::value_type_t<P> v) -> void
{
    m.m_impl->maps.set(id, std::move(v));
}

template <class P>
auto
parameter_maps_access<P>::set(
        parameter_maps& m,
        parameter::id_value_map_t<P> const& vs) -> void
{
    m.m_impl->maps.set(vs);
}

template struct parameter_maps_access<bool_parameter>;
template struct parameter_maps_access<float_parameter>;
template struct parameter_maps_access<int_parameter>;
template struct parameter_maps_access<stereo_level_parameter>;

} // namespace detail

} // namespace piejam::runtime
