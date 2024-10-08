// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/ladspa_fx/ladspa_fx_module.h>

#include <piejam/ladspa/port_descriptor.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter_factory.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::ladspa_fx
{

namespace
{

struct make_module_parameters
{
    make_module_parameters(parameters_map& params)
        : m_params{params}
    {
    }

    auto operator()(std::span<piejam::ladspa::port_descriptor const>
                            control_inputs) const
    {
        fx::module_parameters module_params;

        for (auto const& port_desc : control_inputs)
        {
            std::visit(
                    [&](auto const& p) {
                        (*this)(module_params, port_desc, p);
                    },
                    port_desc.type_desc);
        }

        return module_params;
    }

private:
    void operator()(
            fx::module_parameters& module_params,
            ladspa::port_descriptor const& port_desc,
            ladspa::float_port const p) const
    {
        parameter_factory params_factory{m_params};

        bool const logarithmic = p.logarithmic && p.min > 0.f && p.max > 0.f;

        module_params.emplace(
                port_desc.index,
                params_factory.make_parameter(float_parameter{
                        .name = box(port_desc.name),
                        .default_value = p.default_value,
                        .min = p.min,
                        .max = p.max,
                        .to_normalized =
                                logarithmic ? &parameter::to_normalized_log
                                            : &parameter::to_normalized_linear,
                        .from_normalized =
                                logarithmic
                                        ? &parameter::from_normalized_log
                                        : &parameter::from_normalized_linear}));
    }

    void operator()(
            fx::module_parameters& module_params,
            ladspa::port_descriptor const& port_desc,
            ladspa::int_port const p) const
    {
        parameter_factory params_factory{m_params};

        BOOST_ASSERT(!p.logarithmic);

        module_params.emplace(
                port_desc.index,
                params_factory.make_parameter(int_parameter{
                        .name = box(port_desc.name),
                        .default_value = p.default_value,
                        .min = p.min,
                        .max = p.max}));
    }

    void operator()(
            fx::module_parameters& module_params,
            ladspa::port_descriptor const& port_desc,
            ladspa::bool_port const p) const
    {
        parameter_factory params_factory{m_params};

        module_params.emplace(
                port_desc.index,
                params_factory.make_parameter(bool_parameter{
                        .name = box(port_desc.name),
                        .default_value = p.default_value}));
    }

    parameters_map& m_params;
};

} // namespace

auto
make_module(
        ladspa::instance_id instance_id,
        std::string const& name,
        audio::bus_type const bus_type,
        std::span<ladspa::port_descriptor const> const control_inputs,
        parameters_map& params) -> fx::module
{
    return fx::module{
            .fx_instance_id = instance_id,
            .name = box(name),
            .bus_type = bus_type,
            .parameters = box(make_module_parameters{params}(control_inputs)),
            .streams = {}};
}

} // namespace piejam::runtime::ladspa_fx
