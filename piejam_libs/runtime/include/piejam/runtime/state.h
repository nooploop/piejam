// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/audio_stream.h>
#include <piejam/runtime/bool_parameter.h>
#include <piejam/runtime/enum_parameter.h>
#include <piejam/runtime/external_audio.h>
#include <piejam/runtime/float_parameter.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/fx/state.h>
#include <piejam/runtime/int_parameter.h>
#include <piejam/runtime/material_color.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/midi_device_config.h>
#include <piejam/runtime/midi_devices.h>
#include <piejam/runtime/mixer.h>
#include <piejam/runtime/parameter/assignment.h>
#include <piejam/runtime/parameter/store.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/root_view_mode.h>
#include <piejam/runtime/selected_sound_card.h>
#include <piejam/runtime/startup_session.h>
#include <piejam/runtime/string_id.h>

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

#include <optional>
#include <span>

namespace piejam::runtime
{

struct state
{
    strings_t strings;
    material_colors_t material_colors;

    box<audio::sound_cards> sound_cards;
    runtime::selected_sound_card selected_sound_card;

    audio::sample_rate sample_rate{};
    audio::period_size period_size{};

    external_audio::state external_audio_state;

    box<midi::device_ids_t> midi_inputs;
    box<midi_devices_t> midi_devices;

    parameter::store params;
    audio_streams_store_t streams;

    fx::registry fx_registry;
    fx::state fx_state;

    mixer::state mixer_state{};

    box<midi_assignments_map> midi_assignments;
    std::optional<parameter_id> midi_learning{};

    bool recording{};
    std::size_t rec_session{};
    std::size_t rec_take{};

    std::size_t xruns{};
    float cpu_load{};

    std::size_t display_rotation{};
    bool on_screen_keyboard_enabled{true};

    runtime::root_view_mode root_view_mode{};
    mixer::channel_id fx_browser_fx_chain_id;

    mixer::channel_id focused_fx_chain_id;
    fx::module_id focused_fx_mod_id;

    std::size_t audio_graph_update_count{};
    std::size_t solo_state_update_count{};

    runtime::startup_session startup_session{runtime::startup_session::last};
    box<std::filesystem::path> current_session;
    bool session_modified{false};
};

auto make_initial_state() -> state;

auto add_external_audio_device(
    state&,
    std::string name,
    io_direction,
    audio::bus_type) -> external_audio::device_id;

auto add_mixer_channel(
    state&,
    mixer::channel_type,
    std::string name,
    bool default_record = false) -> mixer::channel_id;

void remove_mixer_channel(state&, mixer::channel_id);

void remove_external_audio_device(state&, external_audio::device_id);

void apply_parameter_values(
    std::span<parameter_value_assignment const>,
    parameters_map const&,
    parameter::store&);

void apply_midi_assignments(
    std::span<parameter_midi_assignment const>,
    parameters_map const&,
    midi_assignments_map&);

auto insert_internal_fx_module(
    state&,
    mixer::channel_id channel_id,
    std::size_t position,
    fx::internal_id,
    std::span<parameter_value_assignment const> initial_values,
    std::span<parameter_midi_assignment const> midi_assigns) -> fx::module_id;

auto insert_ladspa_fx_module(
    state&,
    mixer::channel_id channel_id,
    std::size_t position,
    ladspa::instance_id,
    ladspa::plugin_descriptor const&,
    std::span<ladspa::port_descriptor const> control_inputs,
    std::span<parameter_value_assignment const> initial_values,
    std::span<parameter_midi_assignment const> midi_assigns) -> fx::module_id;

auto insert_missing_ladspa_fx_module(
    state&,
    mixer::channel_id,
    std::size_t position,
    fx::unavailable_ladspa const&,
    std::string_view name) -> fx::module_id;

void remove_fx_module(
    state&,
    mixer::channel_id fx_chain_id,
    fx::module_id fx_mod_id);

void update_midi_assignments(state&, midi_assignments_map const&);

void reset_state(state&);

} // namespace piejam::runtime
