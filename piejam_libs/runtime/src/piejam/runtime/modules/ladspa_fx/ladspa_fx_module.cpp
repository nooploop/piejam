// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/ladspa_fx/ladspa_fx_module.h>

#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::ladspa_fx
{

static auto
make_module_parameters(
        std::span<piejam::ladspa::port_descriptor const> control_inputs,
        ui_parameter_factory const& ui_params_factory) -> fx::module_parameters
{
    fx::module_parameters module_params;

    for (auto const& port_desc : control_inputs)
    {
        if (auto const* const p = std::get_if<piejam::ladspa::float_port>(
                    &port_desc.type_desc))
        {
            bool const logarithmic =
                    p->logarithmic && p->min > 0.f && p->max > 0.f;

            module_params.emplace(
                    port_desc.index,
                    ui_params_factory.make_parameter(
                            float_parameter{
                                    .default_value = p->default_value,
                                    .min = p->min,
                                    .max = p->max,
                                    .to_normalized =
                                            logarithmic
                                                    ? &runtime::parameter::
                                                              to_normalized_log
                                                    : &runtime::parameter::
                                                              to_normalized_linear,
                                    .from_normalized =
                                            logarithmic
                                                    ? &runtime::parameter::
                                                              from_normalized_log
                                                    : &runtime::parameter::
                                                              from_normalized_linear},
                            {.name = port_desc.name,
                             .value_to_string =
                                     &float_parameter_value_to_string}));
        }
        else if (
                auto const* const p = std::get_if<piejam::ladspa::int_port>(
                        &port_desc.type_desc))
        {
            BOOST_ASSERT(!p->logarithmic);

            module_params.emplace(
                    port_desc.index,
                    ui_params_factory.make_parameter(
                            int_parameter{
                                    .default_value = p->default_value,
                                    .min = p->min,
                                    .max = p->max},
                            {.name = port_desc.name,
                             .value_to_string =
                                     &int_parameter_value_to_string}));
        }
        else if (
                auto const* const p = std::get_if<piejam::ladspa::bool_port>(
                        &port_desc.type_desc))
        {
            module_params.emplace(
                    port_desc.index,
                    ui_params_factory.make_parameter(
                            bool_parameter{.default_value = p->default_value},
                            {.name = port_desc.name,
                             .value_to_string =
                                     &bool_parameter_value_to_string}));
        }
    }

    return module_params;
}

auto
make_module(
        ladspa::instance_id instance_id,
        std::string const& name,
        audio::bus_type const bus_type,
        std::span<const ladspa::port_descriptor> const control_inputs,
        ui_parameter_factory const& ui_params_factory) -> fx::module
{
    return fx::module{
            .fx_instance_id = instance_id,
            .name = name,
            .bus_type = bus_type,
            .parameters =
                    make_module_parameters(control_inputs, ui_params_factory),
            .streams = {}};
}

} // namespace piejam::runtime::modules::ladspa_fx
