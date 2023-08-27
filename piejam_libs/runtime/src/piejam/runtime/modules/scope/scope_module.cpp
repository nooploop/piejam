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
#include <piejam/runtime/parameter/generic_value.h>
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
    using namespace std::string_literals;

    if constexpr (BT == audio::bus_type::stereo)
    {
        switch (n)
        {
            case to_underlying(mode::free):
                return "Free"s;
            case to_underlying(mode::trigger_a):
                return "Trigger A"s;
            case to_underlying(mode::trigger_b):
                return "Trigger B"s;

            default:
                return "ERROR";
        }
    }
    else
    {
        switch (n)
        {
            case to_underlying(mode::free):
                return "Free"s;
            case to_underlying(mode::trigger_a):
                return "Trigger"s;

            default:
                return "ERROR";
        }
    }
}

auto
to_trigger_slope_string(int const n) -> std::string
{
    using namespace std::string_literals;

    switch (n)
    {
        case to_underlying(trigger_slope::rising_edge):
            return "Rising Edge"s;

        case to_underlying(trigger_slope::falling_edge):
            return "Falling Edge"s;

        default:
            return "ERROR";
    }
}

auto
to_hold_time_string(float const time) -> std::string
{
    return time > 1000.f ? fmt::format("{:1.2f} s", time / 1000.f)
                         : fmt::format("{:.0f} ms", time);
}

auto
to_window_size_string(int const n) -> std::string
{
    using namespace std::string_literals;

    switch (n)
    {
        case to_underlying(window_size::very_small):
            return "Very Small"s;

        case to_underlying(window_size::small):
            return "Small"s;

        case to_underlying(window_size::middle):
            return "Middle"s;

        case to_underlying(window_size::large):
            return "Large"s;

        case to_underlying(window_size::very_large):
            return "Very Large"s;

        default:
            return "ERROR"s;
    }
}

auto
to_stereo_channel_string(int const n) -> std::string
{
    using namespace std::string_literals;

    switch (n)
    {
        case to_underlying(stereo_channel::left):
            return "L"s;

        case to_underlying(stereo_channel::right):
            return "R"s;

        case to_underlying(stereo_channel::middle):
            return "M"s;

        case to_underlying(stereo_channel::side):
            return "S"s;

        default:
            return "ERROR"s;
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
                                             .name = "Mode",
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
                                             .name = "Slope",
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
                                                     make_default_float_parameter_value_to_string()})},
                            {to_underlying(parameter_key::hold_time),
                             fx_params_factory.make_parameter(
                                     float_parameter{
                                             .default_value = 80.f,
                                             .min = 16.f,
                                             .max = 1600.f,
                                             .to_normalized =
                                                     &parameter::
                                                             to_normalized_linear,
                                             .from_normalized =
                                                     &parameter::
                                                             from_normalized_linear},
                                     fx::parameter{
                                             .name = "Hold Time",
                                             .value_to_string =
                                                     &to_hold_time_string})},
                            {to_underlying(parameter_key::waveform_window_size),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     window_size::large),
                                             .min = to_underlying(
                                                     window_size::very_small),
                                             .max = to_underlying(
                                                     window_size::very_large)},
                                     fx::parameter{
                                             .name = "Window Size",
                                             .value_to_string =
                                                     &to_window_size_string})},
                            {to_underlying(parameter_key::scope_window_size),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     window_size::very_small),
                                             .min = to_underlying(
                                                     window_size::very_small),
                                             .max = to_underlying(
                                                     window_size::very_large)},
                                     fx::parameter{
                                             .name = "Window Size",
                                             .value_to_string =
                                                     &to_window_size_string})},
                            {to_underlying(parameter_key::stream_a_active),
                             fx_params_factory.make_parameter(
                                     bool_parameter{.default_value = true},
                                     fx::parameter{
                                             .name = "Stream A Active",
                                             .value_to_string = fx::
                                                     make_default_bool_parameter_value_to_string()})},
                            {to_underlying(parameter_key::stream_b_active),
                             fx_params_factory.make_parameter(
                                     bool_parameter{.default_value = false},
                                     fx::parameter{
                                             .name = "Stream B Active",
                                             .value_to_string = fx::
                                                     make_default_bool_parameter_value_to_string()})},
                            {to_underlying(parameter_key::channel_a),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     stereo_channel::left),
                                             .min = to_underlying(
                                                     stereo_channel::_min),
                                             .max = to_underlying(
                                                     stereo_channel::_max)},
                                     fx::parameter{
                                             .name = "Channel A",
                                             .value_to_string =
                                                     &to_stereo_channel_string})},
                            {to_underlying(parameter_key::channel_b),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     stereo_channel::right),
                                             .min = to_underlying(
                                                     stereo_channel::_min),
                                             .max = to_underlying(
                                                     stereo_channel::_max)},
                                     fx::parameter{
                                             .name = "Channel B",
                                             .value_to_string =
                                                     &to_stereo_channel_string})},
                    },
            .streams = fx::module_streams{
                    {to_underlying(stream_key::input),
                     streams.add(audio_stream_buffer(
                             std::in_place,
                             num_channels(bus_type)))}}};
}

} // namespace piejam::runtime::modules::scope
