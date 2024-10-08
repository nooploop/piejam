// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/scope/scope_module.h>

#include <piejam/fx_modules/scope/scope_internal_id.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::scope
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
to_mode_string(audio::bus_type bus_type)
{
    return bus_type == audio::bus_type::stereo
                   ? &to_mode_string<audio::bus_type::stereo>
                   : &to_mode_string<audio::bus_type::mono>;
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

        case to_underlying(window_size::medium):
            return "Medium"s;

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

struct dB_ival
{
    static constexpr auto min{-24.f};
    static constexpr auto max{24.f};

    static constexpr auto min_gain{std::pow(10.f, min / 20.f)};
    static constexpr auto max_gain{std::pow(10.f, max / 20.f)};

    static constexpr auto to_normalized =
            &runtime::parameter::to_normalized_dB<dB_ival>;
    static constexpr auto from_normalized =
            &runtime::parameter::from_normalized_dB<dB_ival>;
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", std::log10(x) * 20.f);
}

} // namespace

auto
make_module(runtime::internal_fx_module_factory_args const& args)
        -> runtime::fx::module
{
    using namespace std::string_literals;

    runtime::parameter_factory ui_params_factory{args.params, args.ui_params};

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box("Scope"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::mode),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Mode"s),
                                     .default_value = bus_type_to(
                                             args.bus_type,
                                             to_underlying(mode::trigger),
                                             to_underlying(mode::trigger_a)),
                                     .min = to_underlying(mode::_min),
                                     .max = bus_type_to(
                                             args.bus_type,
                                             to_underlying(mode::trigger),
                                             to_underlying(mode::trigger_b))},
                             {.value_to_string =
                                      to_mode_string(args.bus_type)})},
                    {to_underlying(parameter_key::trigger_slope),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Slope"s),
                                     .default_value = to_underlying(
                                             trigger_slope::rising_edge),
                                     .min = to_underlying(trigger_slope::_min),
                                     .max = to_underlying(trigger_slope::_max)},
                             {.value_to_string = &to_trigger_slope_string})},
                    {to_underlying(parameter_key::trigger_level),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .name = box("Trigger Level"s),
                                     .default_value = 0.f,
                                     .min = -1.f,
                                     .max = 1.f,
                                     .to_normalized =
                                             &runtime::parameter::
                                                     to_normalized_linear,
                                     .from_normalized =
                                             &runtime::parameter::
                                                     from_normalized_linear},
                             {.value_to_string =
                                      &runtime::
                                              float_parameter_value_to_string})},
                    {to_underlying(parameter_key::hold_time),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .name = box("Hold Time"s),
                                     .default_value = 80.f,
                                     .min = 16.f,
                                     .max = 1600.f,
                                     .to_normalized =
                                             &runtime::parameter::
                                                     to_normalized_linear,
                                     .from_normalized =
                                             &runtime::parameter::
                                                     from_normalized_linear},
                             {.value_to_string = &to_hold_time_string})},
                    {to_underlying(parameter_key::waveform_window_size),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Window Size"s),
                                     .default_value =
                                             to_underlying(window_size::large),
                                     .min = to_underlying(window_size::_min),
                                     .max = to_underlying(window_size::_max)},
                             {.value_to_string = &to_window_size_string})},
                    {to_underlying(parameter_key::scope_window_size),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Window Size"s),
                                     .default_value = to_underlying(
                                             window_size::very_small),
                                     .min = to_underlying(window_size::_min),
                                     .max = to_underlying(window_size::_max)},
                             {.value_to_string = &to_window_size_string})},
                    {to_underlying(parameter_key::stream_a_active),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{
                                     .name = box("Stream A Active"s),
                                     .default_value = true},
                             {.value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
                    {to_underlying(parameter_key::stream_b_active),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{
                                     .name = box("Stream B Active"s),
                                     .default_value = false},
                             {.value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
                    {to_underlying(parameter_key::channel_a),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Channel A"s),
                                     .default_value = to_underlying(
                                             stereo_channel::left),
                                     .min = to_underlying(stereo_channel::_min),
                                     .max = to_underlying(
                                             stereo_channel::_max)},
                             {.value_to_string = &to_stereo_channel_string})},
                    {to_underlying(parameter_key::channel_b),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .name = box("Channel B"s),
                                     .default_value = to_underlying(
                                             stereo_channel::right),
                                     .min = to_underlying(stereo_channel::_min),
                                     .max = to_underlying(
                                             stereo_channel::_max)},
                             {.value_to_string = &to_stereo_channel_string})},
                    {to_underlying(parameter_key::gain_a),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .name = box("Gain A"s),
                                     .default_value = 1.f,
                                     .min = dB_ival::min_gain,
                                     .max = dB_ival::max_gain,
                                     .to_normalized = dB_ival::to_normalized,
                                     .from_normalized =
                                             dB_ival::from_normalized},
                             {.value_to_string = &to_dB_string})},
                    {to_underlying(parameter_key::gain_b),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .name = box("Gain B"s),
                                     .default_value = 1.f,
                                     .min = dB_ival::min_gain,
                                     .max = dB_ival::max_gain,
                                     .to_normalized = dB_ival::to_normalized,
                                     .from_normalized =
                                             dB_ival::from_normalized},
                             {.value_to_string = &to_dB_string})},
                    {to_underlying(parameter_key::freeze),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{
                                     .name = box("Freeze"s),
                                     .default_value = false},
                             {.value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
            }),
            .streams = box(runtime::fx::module_streams{
                    {to_underlying(stream_key::input),
                     args.streams.emplace(
                             std::in_place,
                             num_channels(args.bus_type))}})};
}

} // namespace piejam::fx_modules::scope
