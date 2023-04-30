// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/filter/filter_module.h>

#include <piejam/entity_map.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx_parameter_factory.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter_maps_access.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::filter
{

namespace
{

static auto
to_type_string(int const n) -> std::string
{
    switch (n)
    {
        case to_underlying(type::lp2):
            return "LP2";
        case to_underlying(type::lp4):
            return "LP4";
        case to_underlying(type::bp2):
            return "BP2";
        case to_underlying(type::bp4):
            return "BP4";
        case to_underlying(type::hp2):
            return "HP2";
        case to_underlying(type::hp4):
            return "HP4";
        case to_underlying(type::br):
            return "BR";

        default:
            return "Pass";
    }
}

static auto
to_cutoff_string(float const f)
{
    return f < 1000.f ? fmt::format("{:.2f} Hz", f)
                      : fmt::format("{:.2f} kHz", f / 1000.f);
}

static auto
to_resonance_string(float const r)
{
    auto p = r * 100;
    return p < 10.f ? fmt::format("{:.2f}%", p)
                    : (p < 100.f ? fmt::format("{:.1f}%", p)
                                 : fmt::format("{:.0f}%", p));
}

} // namespace

auto
make_module(
        audio::bus_type const bus_type,
        fx_parameter_factory const& fx_params_factory,
        audio_streams_cache& streams) -> fx::module
{
    using namespace std::string_literals;

    return fx::module{
            .fx_instance_id = fx::internal::filter,
            .name = "Filter"s,
            .bus_type = bus_type,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(parameter_key::type),
                             fx_params_factory.make_parameter(
                                     runtime::int_parameter{
                                             .default_value =
                                                     to_underlying(type::lp2),
                                             .min = 0,
                                             .max = 7},
                                     fx::parameter{
                                             .name = "Type",
                                             .value_to_string =
                                                     &to_type_string})},
                            {to_underlying(parameter_key::cutoff),
                             fx_params_factory.make_parameter(
                                     runtime::float_parameter{
                                             .default_value = 440.f,
                                             .min = 10.f,
                                             .max = 20000.f,
                                             .to_normalized =
                                                     &runtime::parameter::
                                                             to_normalized_log,
                                             .from_normalized =
                                                     &runtime::parameter::
                                                             from_normalized_log},
                                     fx::parameter{
                                             .name = "Cutoff"s,
                                             .value_to_string =
                                                     &to_cutoff_string})},
                            {to_underlying(parameter_key::resonance),
                             fx_params_factory.make_parameter(
                                     runtime::float_parameter{
                                             .default_value = 0.f,
                                             .min = 0.f,
                                             .max = 1.f,
                                             .to_normalized =
                                                     &runtime::parameter::
                                                             to_normalized_linear,
                                             .from_normalized =
                                                     &runtime::parameter::
                                                             from_normalized_linear},
                                     fx::parameter{
                                             .name = "Resonance"s,
                                             .value_to_string =
                                                     &to_resonance_string})}},
            .streams = fx::module_streams{
                    {to_underlying(stream_key::in_out),
                     streams.add(audio_stream_buffer(std::in_place, 4))}}};
}

} // namespace piejam::runtime::modules::filter
