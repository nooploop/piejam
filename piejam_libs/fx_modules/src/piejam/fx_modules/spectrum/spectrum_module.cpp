// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/spectrum_module.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::spectrum
{

namespace
{

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
            .name = box("Spectrum"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::stream_a_active),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{.default_value = true},
                             {.name = box("Stream A Active"s),
                              .value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
                    {to_underlying(parameter_key::stream_b_active),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{.default_value = false},
                             {.name = box("Stream B Active"s),
                              .value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
                    {to_underlying(parameter_key::channel_a),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .default_value = to_underlying(
                                             stereo_channel::left),
                                     .min = to_underlying(stereo_channel::_min),
                                     .max = to_underlying(
                                             stereo_channel::_max)},
                             {.name = box("Channel A"s),
                              .value_to_string = &to_stereo_channel_string})},
                    {to_underlying(parameter_key::channel_b),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .default_value = to_underlying(
                                             stereo_channel::right),
                                     .min = to_underlying(stereo_channel::_min),
                                     .max = to_underlying(
                                             stereo_channel::_max)},
                             {.name = box("Channel B"s),
                              .value_to_string = &to_stereo_channel_string})},
                    {to_underlying(parameter_key::gain_a),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .default_value = 1.f,
                                     .min = dB_ival::min_gain,
                                     .max = dB_ival::max_gain,
                                     .to_normalized = dB_ival::to_normalized,
                                     .from_normalized =
                                             dB_ival::from_normalized},
                             {.name = box("Gain A"s),
                              .value_to_string = &to_dB_string})},
                    {to_underlying(parameter_key::gain_b),
                     ui_params_factory.make_parameter(
                             runtime::float_parameter{
                                     .default_value = 1.f,
                                     .min = dB_ival::min_gain,
                                     .max = dB_ival::max_gain,
                                     .to_normalized = dB_ival::to_normalized,
                                     .from_normalized =
                                             dB_ival::from_normalized},
                             {.name = box("Gain B"s),
                              .value_to_string = &to_dB_string})},
                    {to_underlying(parameter_key::freeze),
                     ui_params_factory.make_parameter(
                             runtime::bool_parameter{.default_value = false},
                             {.name = box("Freeze"s),
                              .value_to_string =
                                      &runtime::
                                              bool_parameter_value_to_string})},
            }),
            .streams = box(runtime::fx::module_streams{
                    {to_underlying(stream_key::input),
                     args.streams.add(runtime::audio_stream_buffer(
                             std::in_place,
                             audio::num_channels(args.bus_type)))}})};
}

} // namespace piejam::fx_modules::spectrum
