// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>
#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/boxed_vector.h>
#include <piejam/entity_id_hash.h>
#include <piejam/io_direction.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/npos.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/device_io.h>
#include <piejam/runtime/fx/ladspa_instances.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/midi_device_config.h>
#include <piejam/runtime/midi_devices.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/parameter_maps.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/recorder.h>
#include <piejam/runtime/selected_device.h>

#include <functional>
#include <optional>
#include <span>
#include <vector>

namespace piejam::runtime
{

struct state
{
    box<audio::pcm_io_descriptors> pcm_devices;

    selected_device input;
    selected_device output;

    audio::sample_rate sample_rate{};
    audio::period_size period_size{};
    audio::period_count period_count{};

    device_io::state device_io_state;

    boxed_vector<midi::device_id_t> midi_inputs;
    box<midi_devices_t> midi_devices;

    parameter_maps params;
    audio_streams_cache streams;

    fx::registry fx_registry{fx::make_default_registry()};

    fx::modules_t fx_modules;
    box<fx::parameters_t> fx_parameters;
    box<fx::ladspa_instances> fx_ladspa_instances;
    fx::unavailable_ladspa_plugins fx_unavailable_ladspa_plugins;

    mixer::state mixer_state{};

    box<midi_assignments_map> midi_assignments;
    std::optional<midi_assignment_id> midi_learning{};

    bool recording{};
    unique_box<recorder_streams_t> recorder_streams;
    std::size_t rec_session{};
    std::size_t rec_take{};

    std::size_t xruns{};
    float cpu_load{};
};

auto make_initial_state() -> state;

auto sample_rates(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::sample_rates_t;
auto sample_rates_from_state(state const&) -> audio::sample_rates_t;

auto period_sizes(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::period_sizes_t;
auto period_sizes_from_state(state const&) -> audio::period_sizes_t;

auto period_counts(
        box<audio::pcm_hw_params> input_hw_params,
        box<audio::pcm_hw_params> output_hw_params) -> audio::period_counts_t;
auto period_counts_from_state(state const&) -> audio::period_counts_t;

auto add_device_bus(
        state&,
        std::string const& name,
        io_direction,
        audio::bus_type,
        channel_index_pair const&) -> device_io::bus_id;

auto add_mixer_channel(state&, std::string name) -> mixer::channel_id;

void remove_mixer_channel(state&, mixer::channel_id);

void remove_device_bus(state&, device_io::bus_id);

auto insert_internal_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        fx::internal,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> fx::module_id;
void insert_ladspa_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        ladspa::instance_id,
        ladspa::plugin_descriptor const&,
        std::span<ladspa::port_descriptor const> const& control_inputs,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns);
void insert_missing_ladspa_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        fx::unavailable_ladspa const&,
        std::string_view const& name);
void remove_fx_module(state&, fx::module_id fx_mod_id);

void update_midi_assignments(state&, midi_assignments_map const&);

} // namespace piejam::runtime
