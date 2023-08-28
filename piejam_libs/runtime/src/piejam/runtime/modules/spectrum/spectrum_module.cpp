// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/spectrum/spectrum_module.h>

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

namespace piejam::runtime::modules::spectrum
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
};

auto
to_dB_string(float x) -> std::string
{
    return fmt::format("{:.1f} dB", std::log10(x) * 20.f);
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
            .fx_instance_id = fx::internal::spectrum,
            .name = "Spectrum"s,
            .bus_type = bus_type,
            .parameters =
                    fx::module_parameters{
                            {to_underlying(parameter_key::stream_a_active),
                             fx_params_factory.make_parameter(
                                     bool_parameter{.default_value = true},
                                     {.name = "Stream A Active",
                                      .value_to_string =
                                              std::in_place_type<bool>})},
                            {to_underlying(parameter_key::stream_b_active),
                             fx_params_factory.make_parameter(
                                     bool_parameter{.default_value = false},
                                     {.name = "Stream B Active",
                                      .value_to_string =
                                              std::in_place_type<bool>})},
                            {to_underlying(parameter_key::channel_a),
                             fx_params_factory.make_parameter(
                                     int_parameter{
                                             .default_value = to_underlying(
                                                     stereo_channel::left),
                                             .min = to_underlying(
                                                     stereo_channel::_min),
                                             .max = to_underlying(
                                                     stereo_channel::_max)},
                                     {.name = "Channel A",
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
                                     {.name = "Channel B",
                                      .value_to_string =
                                              &to_stereo_channel_string})},
                            {to_underlying(parameter_key::gain_a),
                             fx_params_factory.make_parameter(
                                     float_parameter{
                                             .default_value = 1.f,
                                             .min = std::pow(
                                                     10.f,
                                                     dB_ival::min / 20.f),
                                             .max = std::pow(
                                                     10.f,
                                                     dB_ival::max / 20.f),
                                             .to_normalized =
                                                     &parameter::
                                                             to_normalized_dB<
                                                                     dB_ival>,
                                             .from_normalized =
                                                     &runtime::parameter::
                                                             from_normalized_dB<
                                                                     dB_ival>},
                                     {.name = "Gain A"s,
                                      .value_to_string = &to_dB_string})},
                            {to_underlying(parameter_key::gain_b),
                             fx_params_factory.make_parameter(
                                     float_parameter{
                                             .default_value = 1.f,
                                             .min = std::pow(
                                                     10.f,
                                                     dB_ival::min / 20.f),
                                             .max = std::pow(
                                                     10.f,
                                                     dB_ival::max / 20.f),
                                             .to_normalized =
                                                     &runtime::parameter::
                                                             to_normalized_dB<
                                                                     dB_ival>,
                                             .from_normalized =
                                                     &parameter::
                                                             from_normalized_dB<
                                                                     dB_ival>},
                                     {.name = "Gain B"s,
                                      .value_to_string = &to_dB_string})},
                            {to_underlying(parameter_key::freeze),
                             fx_params_factory.make_parameter(
                                     bool_parameter{.default_value = false},
                                     {.name = "Freeze",
                                      .value_to_string =
                                              std::in_place_type<bool>})},
                    },
            .streams = fx::module_streams{
                    {to_underlying(stream_key::input),
                     streams.add(audio_stream_buffer(
                             std::in_place,
                             audio::num_channels(bus_type)))}}};
}

} // namespace piejam::runtime::modules::spectrum
