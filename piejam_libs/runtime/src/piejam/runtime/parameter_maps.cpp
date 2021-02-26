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

namespace piejam::runtime
{

auto
get_parameter_value(parameter_maps const& m, float_parameter_id const id)
        -> float const*
{
    return m.get(id);
}

auto
get_parameter_value(parameter_maps const& m, int_parameter_id const id)
        -> int const*
{
    return m.get(id);
}

auto
get_parameter_value(parameter_maps const& m, bool_parameter_id const id)
        -> bool const*
{
    return m.get(id);
}

} // namespace piejam::runtime
