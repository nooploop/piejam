// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/registry_map.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <functional>
#include <memory>
#include <string_view>

namespace piejam::runtime
{

struct internal_fx_component_factory_args
{
    fx::module const& fx_mod;
    audio::sample_rate sample_rate;
    parameter_processor_factory& param_procs;
    processors::stream_processor_factory& stream_procs;
    std::string_view name;
};

using internal_fx_component_factory =
        std::function<std::unique_ptr<audio::engine::component>(
                internal_fx_component_factory_args const&)>;

using internal_fx_component_factories = registry_map<
        struct internal_fx_component_factories_tag,
        fx::internal_id,
        internal_fx_component_factory>;

} // namespace piejam::runtime
