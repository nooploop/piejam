// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/fx_scope.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/runtime/components/stereo_stream.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/scope.h>
#include <piejam/to_underlying.h>

#include <boost/container/flat_map.hpp>

#include <array>

namespace piejam::runtime::components
{

namespace
{

constexpr auto
stream_capacity(audio::sample_rate const sample_rate) -> std::size_t
{
    // 3 x 17ms
    return static_cast<std::size_t>((sample_rate.as_float() / 1000.f) * 17 * 3);
}

} // namespace

auto
make_fx_scope(
        fx::module const& fx_mod,
        audio::sample_rate const& sample_rate,
        processors::stream_processor_factory& stream_proc_factory)
        -> std::unique_ptr<audio::engine::component>
{
    return components::make_stereo_stream(
            fx_mod.streams->at(to_underlying(fx::scope_stream_key::left_right)),
            stream_proc_factory,
            stream_capacity(sample_rate),
            "scope");
}

} // namespace piejam::runtime::components
