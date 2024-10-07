// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tuner/tuner_module.h>

#include <piejam/fx_modules/tuner/tuner_internal_id.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter_factory.h>
#include <piejam/runtime/parameter_value_to_string.h>
#include <piejam/runtime/ui_parameter_descriptors_map.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::tuner
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

        default:
            return "ERROR"s;
    }
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
            .name = box("Tuner"s),
            .bus_type = args.bus_type,
            .parameters = box(runtime::fx::module_parameters{
                    {to_underlying(parameter_key::channel),
                     ui_params_factory.make_parameter(
                             runtime::int_parameter{
                                     .default_value = to_underlying(
                                             stereo_channel::middle),
                                     .min = to_underlying(stereo_channel::_min),
                                     .max = to_underlying(
                                             stereo_channel::_max)},
                             {.name = box("Channel"s),
                              .value_to_string = &to_stereo_channel_string})},
            }),
            .streams = box(runtime::fx::module_streams{
                    {to_underlying(stream_key::input),
                     args.streams.emplace(
                             std::in_place,
                             audio::num_channels(args.bus_type))}})};
}

} // namespace piejam::fx_modules::tuner
