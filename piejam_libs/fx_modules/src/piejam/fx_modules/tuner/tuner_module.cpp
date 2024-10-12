// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/tuner/tuner_module.h>

#include <piejam/fx_modules/tuner/tuner_internal_id.h>

#include <piejam/audio/multichannel_buffer.h>
#include <piejam/entity_map.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::tuner
{

auto
make_module(runtime::internal_fx_module_factory_args const& args)
        -> runtime::fx::module
{
    using namespace std::string_literals;

    return runtime::fx::module{
            .fx_instance_id = internal_id(),
            .name = box("Tuner"s),
            .bus_type = args.bus_type,
            .parameters = {},
            .streams = box(runtime::fx::module_streams{
                    {to_underlying(stream_key::input),
                     make_stream(
                             args.streams,
                             audio::num_channels(args.bus_type))},
            })};
}

} // namespace piejam::fx_modules::tuner
