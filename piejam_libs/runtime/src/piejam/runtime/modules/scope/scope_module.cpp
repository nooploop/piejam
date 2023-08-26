// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/scope/scope_module.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx_parameter_factory.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::scope
{

namespace
{

template <audio::bus_type BT>
auto
to_mode_string(int const n) -> std::string
{
    if constexpr (BT == audio::bus_type::stereo)
    {
        switch (n)
        {
            case to_underlying(mode::free):
                return "Free";
            case to_underlying(mode::trigger_a):
                return "Trigger A";
            case to_underlying(mode::trigger_b):
                return "Trigger B";

            default:
                return "ERROR";
        }
    }
    else
    {
        switch (n)
        {
            case to_underlying(mode::free):
                return "Free";
            case to_underlying(mode::trigger_a):
                return "Trigger A";

            default:
                return "ERROR";
        }
    }
}

auto
to_trigger_slope_string(int const n) -> std::string
{
    switch (n)
    {
        case to_underlying(trigger_slope::rising_edge):
            return "Rising Edge";

        case to_underlying(trigger_slope::falling_edge):
            return "Falling Edge";

        default:
            return "ERROR";
    }
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
            .fx_instance_id = fx::internal::scope,
            .name = "Scope"s,
            .bus_type = bus_type,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(parameter_key::mode),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     mode::trigger_a),
                                             .min = 0,
                                             .max = bus_type == audio::bus_type::
                                                                            stereo
                                                            ? 2
                                                            : 1},
                                     fx::parameter{
                                             .name = "Trigger Source",
                                             .value_to_string =
                                                     bus_type == audio::bus_type::
                                                                             stereo
                                                             ? &to_mode_string<
                                                                       audio::bus_type::
                                                                               stereo>
                                                             : &to_mode_string<
                                                                       audio::bus_type::
                                                                               mono>})},
                            {to_underlying(parameter_key::trigger_slope),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     trigger_slope::
                                                             rising_edge),
                                             .min = 0,
                                             .max = 1},
                                     fx::parameter{
                                             .name = "Trigger Slope",
                                             .value_to_string =
                                                     &to_trigger_slope_string})},
                            {to_underlying(parameter_key::trigger_level),
                             fx_params_factory.make_parameter(
                                     float_parameter{
                                             .default_value = 0.f,
                                             .min = -1.f,
                                             .max = 1.f,
                                             .to_normalized =
                                                     &parameter::
                                                             to_normalized_linear,
                                             .from_normalized =
                                                     &parameter::
                                                             from_normalized_linear},
                                     fx::parameter{
                                             .name = "Trigger Level",
                                             .value_to_string = fx::
                                                     make_default_float_parameter_value_to_string()})}},
            .streams = fx::module_streams{
                    {to_underlying(stream_key::input),
                     streams.add(audio_stream_buffer(
                             std::in_place,
                             num_channels(bus_type)))}}};
}

} // namespace piejam::runtime::modules::scope
