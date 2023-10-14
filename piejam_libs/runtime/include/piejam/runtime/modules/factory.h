// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/sample_rate.h>
#include <piejam/audio/types.h>
#include <piejam/registry_map.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::runtime::modules
{

struct internal_fx_module_factory_args
{
    audio::bus_type bus_type;
    audio::sample_rate sample_rate;
    parameters_map& params;
    ui_parameter_descriptors_map& ui_params;
    audio_streams_cache& streams;
};

using internal_fx_module_factory =
        std::function<fx::module(internal_fx_module_factory_args const&)>;

using internal_fx_module_factories = registry_map<
        struct internal_fx_module_factory_tag,
        fx::internal_id,
        internal_fx_module_factory>;

} // namespace piejam::runtime::modules
