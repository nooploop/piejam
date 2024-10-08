// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/spectrum_module.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/fx_modules/spectrum/spectrum_internal_id.h>
#include <piejam/math.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/float_normalize.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter_factory.h>
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

    static constexpr auto min_gain{math::from_dB(min)};
    static constexpr auto max_gain{math::from_dB(max)};

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

    runtime::parameter_factory params_factory{args.params};

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box("Spectrum"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::stream_a_active),
                     params_factory.make_parameter(runtime::bool_parameter{
                             .name = box("Stream A Active"s),
                             .default_value = true})},
                    {to_underlying(parameter_key::stream_b_active),
                     params_factory.make_parameter(runtime::bool_parameter{
                             .name = box("Stream B Active"s),
                             .default_value = false})},
                    {to_underlying(parameter_key::channel_a),
                     params_factory.make_parameter(runtime::int_parameter{
                             .name = box("Channel A"s),
                             .default_value =
                                     to_underlying(stereo_channel::left),
                             .min = to_underlying(stereo_channel::_min),
                             .max = to_underlying(stereo_channel::_max),
                             .value_to_string = &to_stereo_channel_string,
                     })},
                    {to_underlying(parameter_key::channel_b),
                     params_factory.make_parameter(runtime::int_parameter{
                             .name = box("Channel B"s),
                             .default_value =
                                     to_underlying(stereo_channel::right),
                             .min = to_underlying(stereo_channel::_min),
                             .max = to_underlying(stereo_channel::_max),
                             .value_to_string = &to_stereo_channel_string})},
                    {to_underlying(parameter_key::gain_a),
                     params_factory.make_parameter(runtime::float_parameter{
                             .name = box("Gain A"s),
                             .default_value = 1.f,
                             .min = dB_ival::min_gain,
                             .max = dB_ival::max_gain,
                             .value_to_string = &to_dB_string,
                             .to_normalized = dB_ival::to_normalized,
                             .from_normalized = dB_ival::from_normalized})},
                    {to_underlying(parameter_key::gain_b),
                     params_factory.make_parameter(runtime::float_parameter{
                             .name = box("Gain B"s),
                             .default_value = 1.f,
                             .min = dB_ival::min_gain,
                             .max = dB_ival::max_gain,
                             .value_to_string = &to_dB_string,
                             .to_normalized = dB_ival::to_normalized,
                             .from_normalized = dB_ival::from_normalized})},
                    {to_underlying(parameter_key::freeze),
                     params_factory.make_parameter(runtime::bool_parameter{
                             .name = box("Freeze"s),
                             .default_value = false})},
            }),
            .streams = box(runtime::fx::module_streams{
                    {to_underlying(stream_key::input),
                     args.streams.emplace(
                             std::in_place,
                             audio::num_channels(args.bus_type))}})};
}

} // namespace piejam::fx_modules::spectrum
