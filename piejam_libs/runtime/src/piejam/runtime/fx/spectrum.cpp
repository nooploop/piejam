// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/spectrum.h>

#include <piejam/entity_map.h>
#include <piejam/runtime/fx/internal.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/to_underlying.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::fx
{

auto
make_spectrum_module(audio_streams_cache& streams) -> module
{
    using namespace std::string_literals;

    return module{
            .fx_instance_id = internal::spectrum,
            .name = "Spectrum Analyzer"s,
            .parameters = fx::module_parameters{},
            .streams = module_streams{
                    {to_underlying(spectrum_stream_key::left_right),
                     streams.add(audio_stream_buffer(std::in_place, 2))}}};
}

} // namespace piejam::runtime::fx
