// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/sound_card_hw_params.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/boxed_vector.h>
#include <piejam/entity_data_map.h>
#include <piejam/entity_id_hash.h>
#include <piejam/io_direction.h>
#include <piejam/ladspa/fwd.h>
#include <piejam/npos.h>
#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/external_audio.h>
#include <piejam/runtime/fx/ladspa_instances.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/material_color.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/midi_device_config.h>
#include <piejam/runtime/midi_devices.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/assignment.h>
#include <piejam/runtime/parameter/bool_descriptor.h>
#include <piejam/runtime/parameter/float_descriptor.h>
#include <piejam/runtime/parameter/int_descriptor.h>
#include <piejam/runtime/parameter/map.h>
#include <piejam/runtime/parameter/stereo_level_descriptor.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/parameters_map.h>
#include <piejam/runtime/root_view_mode.h>
#include <piejam/runtime/selected_sound_card.h>

#include <optional>
#include <span>
#include <vector>

namespace piejam::runtime
{

struct state
{
    std::size_t reduce_count{};

    audio::io_sound_cards io_sound_cards;
    io_pair<selected_sound_card> selected_io_sound_card;

    audio::sample_rate sample_rate{};
    audio::period_size period_size{};
    audio::period_count period_count{};

    external_audio::state external_audio_state;

    box<midi::device_ids_t> midi_inputs;
    box<midi_devices_t> midi_devices;

    parameters_map params;
    audio_streams_t streams;

    fx::registry fx_registry;

    fx::modules_t fx_modules;
    fx::ladspa_instances fx_ladspa_instances;
    fx::unavailable_ladspa_plugins fx_unavailable_ladspa_plugins;

    mixer::state mixer_state{};

    box<midi_assignments_map> midi_assignments;
    std::optional<midi_assignment_id> midi_learning{};

    bool recording{};
    std::size_t rec_session{};
    std::size_t rec_take{};

    std::size_t xruns{};
    float cpu_load{};

    struct
    {
        root_view_mode root_view_mode_{};

        mixer::channel_id fx_browser_fx_chain_id;

        mixer::channel_id focused_fx_chain_id;
        fx::module_id focused_fx_mod_id;

        entity_data_map<mixer::channel_id, material_color> mixer_colors;
    } gui_state;
};

auto make_initial_state() -> state;

auto sample_rates(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::sample_rates_t;
auto sample_rates_from_state(state const&) -> audio::sample_rates_t;

auto period_sizes(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::period_sizes_t;
auto period_sizes_from_state(state const&) -> audio::period_sizes_t;

auto period_counts(
        box<audio::sound_card_hw_params> const& input_hw_params,
        box<audio::sound_card_hw_params> const& output_hw_params)
        -> audio::period_counts_t;
auto period_counts_from_state(state const&) -> audio::period_counts_t;

auto add_external_audio_device(
        state&,
        std::string const& name,
        io_direction,
        audio::bus_type,
        channel_index_pair const&) -> external_audio::device_id;

auto add_mixer_channel(state&, std::string name, audio::bus_type)
        -> mixer::channel_id;

void remove_mixer_channel(state&, mixer::channel_id);

void remove_external_audio_device(state&, external_audio::device_id);

void apply_parameter_values(
        std::vector<fx::parameter_value_assignment> const&,
        fx::module const&,
        parameters_map&);

void apply_fx_midi_assignments(
        std::vector<fx::parameter_midi_assignment> const&,
        fx::module const&,
        midi_assignments_map&);

auto insert_internal_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        fx::internal_id,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> fx::module_id;
auto insert_ladspa_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        ladspa::instance_id,
        ladspa::plugin_descriptor const&,
        std::span<ladspa::port_descriptor const> control_inputs,
        std::vector<fx::parameter_value_assignment> const& initial_values,
        std::vector<fx::parameter_midi_assignment> const& midi_assigns)
        -> fx::module_id;
void insert_missing_ladspa_fx_module(
        state&,
        mixer::channel_id,
        std::size_t position,
        fx::unavailable_ladspa const&,
        std::string_view name);
void remove_fx_module(
        state&,
        mixer::channel_id fx_chain_id,
        fx::module_id fx_mod_id);

void update_midi_assignments(state&, midi_assignments_map const&);

} // namespace piejam::runtime
