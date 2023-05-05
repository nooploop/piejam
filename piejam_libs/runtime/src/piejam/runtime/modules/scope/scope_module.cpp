// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/scope/scope_module.h>

#include <piejam/entity_map.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/to_underlying.h>

#include <piejam/audio/multichannel_buffer.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::scope
{

auto
make_module(audio::bus_type const bus_type, audio_streams_cache& streams)
        -> fx::module
{
    using namespace std::string_literals;

    return fx::module{
            .fx_instance_id = fx::internal::scope,
            .name = "Oscilloscope"s,
            .bus_type = bus_type,
            .parameters = fx::module_parameters{},
            .streams = fx::module_streams{
                    {to_underlying(stream_key::input),
                     streams.add(audio_stream_buffer(std::in_place, 2))}}};
}

} // namespace piejam::runtime::modules::scope
