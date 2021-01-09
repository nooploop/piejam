// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/entity_id_hash.h>
#include <piejam/io_direction.h>
#include <piejam/npos.h>
#include <piejam/runtime/fx/ladspa_instances.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter/maps_collection.h>
#include <piejam/runtime/parameter_maps.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/selected_device.h>

#include <functional>
#include <span>
#include <vector>

namespace piejam::runtime
{

struct state
{
    box<audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    audio::samplerate_t samplerate{};
    audio::period_size_t period_size{};

    parameter_maps params;

    fx::registry fx_registry{fx::make_default_registry()};

    fx::modules_t fx_modules;
    box<fx::parameters_t> fx_parameters;
    box<fx::ladspa_instances> fx_ladspa_instances;
    fx::unavailable_ladspa_plugins fx_unavailable_ladspa_plugins;

    mixer::state mixer_state{};

    mixer::bus_id fx_chain_bus{};

    std::size_t xruns{};
    float cpu_load{};
};

auto samplerates(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::samplerates_t;
auto samplerates_from_state(state const&) -> audio::samplerates_t;

auto period_sizes(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::period_sizes_t;
auto period_sizes_from_state(state const&) -> audio::period_sizes_t;

template <io_direction D>
auto add_mixer_bus(
        state&,
        std::string name,
        audio::bus_type type,
        channel_index_pair const& chs = channel_index_pair{npos})
        -> mixer::bus_id;

void remove_mixer_bus(state&, mixer::bus_id);

template <io_direction D>
void clear_mixer_buses(state&);

auto insert_internal_fx_module(
        state&,
        mixer::bus_id,
        std::size_t position,
        fx::internal,
        std::vector<fx::parameter_assignment> const& initial_assignments)
        -> fx::module_id;
void insert_ladspa_fx_module(
        state&,
        mixer::bus_id,
        std::size_t position,
        fx::ladspa_instance_id,
        audio::ladspa::plugin_descriptor const&,
        std::span<audio::ladspa::port_descriptor const> const& control_inputs,
        std::vector<fx::parameter_assignment> const& initial_assignments);
void insert_missing_ladspa_fx_module(
        state&,
        mixer::bus_id,
        std::size_t position,
        fx::unavailable_ladspa const&,
        std::string_view const& name);
void remove_fx_module(state& st, fx::module_id id);

} // namespace piejam::runtime
