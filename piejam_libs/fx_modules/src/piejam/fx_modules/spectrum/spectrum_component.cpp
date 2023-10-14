// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/fx_modules/spectrum/spectrum_component.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/endpoint_ports.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/fx_modules/spectrum/spectrum_module.h>
#include <piejam/runtime/components/stream.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::fx_modules::spectrum
{

auto
make_component(
        runtime::components::internal_fx_component_factory_args const& args)
        -> std::unique_ptr<audio::engine::component>
{
    return runtime::components::make_stream(
            args.fx_mod.streams->at(to_underlying(stream_key::input)),
            args.stream_procs,
            num_channels(args.fx_mod.bus_type),
            args.sample_rate.to_samples(std::chrono::milliseconds(120)),
            "spectrum");
}

} // namespace piejam::fx_modules::spectrum
