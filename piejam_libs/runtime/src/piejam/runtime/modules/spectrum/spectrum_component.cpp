// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/modules/spectrum/spectrum_component.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/runtime/components/stereo_stream.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/modules/spectrum/spectrum_module.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

namespace piejam::runtime::modules::spectrum
{

auto
make_component(
        fx::module const& fx_mod,
        audio::sample_rate const sample_rate,
        processors::stream_processor_factory& stream_proc_factory)
        -> std::unique_ptr<audio::engine::component>
{
    return components::make_stereo_stream(
            fx_mod.streams->at(to_underlying(stream_key::left_right)),
            stream_proc_factory,
            sample_rate.to_samples(std::chrono::milliseconds(17 * 3)),
            "spectrum");
}

} // namespace piejam::runtime::modules::spectrum
