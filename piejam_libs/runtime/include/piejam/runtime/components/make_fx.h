// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/registry_map.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/get_parameter_name.h>
#include <piejam/runtime/fx/ladspa_processor_factory.h>

#include <functional>
#include <memory>
#include <string_view>

namespace piejam::runtime::components
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

auto
make_fx(fx::module const&,
        fx::get_parameter_name const&,
        fx::simple_ladspa_processor_factory const&,
        parameter_processor_factory&,
        processors::stream_processor_factory&,
        audio::sample_rate,
        std::string_view name = {})
        -> std::unique_ptr<audio::engine::component>;

} // namespace piejam::runtime::components
