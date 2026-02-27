// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/material_color.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>
#include <piejam/runtime/string_id.h>

#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>
#include <piejam/audio/sound_card_descriptor.h>
#include <piejam/audio/types.h>
#include <piejam/box.h>
#include <piejam/boxed_string.h>
#include <piejam/boxed_vector.h>
#include <piejam/entity_id.h>
#include <piejam/io_direction.h>
#include <piejam/midi/device_id.h>
#include <piejam/redux/fwd.h>

#include <cstddef>
#include <filesystem>
#include <optional>

namespace piejam::runtime::selectors
{

template <class Value>
using selector = redux::selector<Value, state>;

auto make_string_selector(string_id) -> selector<boxed_string>;

template <class Available, class Current>
struct choice_model
{
    Available available;
    Current current;

    auto operator==(choice_model const&) const -> bool = default;
};

using sample_rate_choice =
    choice_model<audio::sample_rates_t, audio::sample_rate>;
extern selector<box<sample_rate_choice>> const select_sample_rate;

using period_size_choice =
    choice_model<audio::period_sizes_t, audio::period_size>;
extern selector<box<period_size_choice>> const select_period_size;

extern selector<float> const select_buffer_latency;

struct sound_card_info
{
    std::string name;
    unsigned num_ins{};
    unsigned num_outs{};

    auto operator==(sound_card_info const&) const -> bool = default;
};

using sound_card_choice =
    choice_model<std::vector<sound_card_info>, std::size_t>;
extern selector<box<sound_card_choice>> const select_sound_card;

auto make_num_device_channels_selector(io_direction) -> selector<std::size_t>;

auto make_external_audio_device_ids_selector(io_direction)
    -> selector<box<external_audio::device_ids_t>>;

auto make_external_audio_device_name_selector(external_audio::device_id)
    -> selector<string_id>;

auto make_external_audio_device_bus_type_selector(external_audio::device_id)
    -> selector<audio::bus_type>;

auto make_external_audio_device_bus_channel_selector(
    external_audio::device_id,
    audio::bus_channel) -> selector<std::size_t>;

extern selector<box<mixer::channel_ids_t>> const select_mixer_aux_channels;
extern selector<box<mixer::channel_ids_t>> const select_mixer_user_channels;
extern selector<mixer::channel_id> const select_mixer_main_channel;

auto make_mixer_channel_type_selector(mixer::channel_id)
    -> selector<mixer::channel_type>;
auto make_mixer_channel_color_selector(mixer::channel_id)
    -> selector<material_color>;
auto make_mixer_channel_volume_parameter_selector(mixer::channel_id)
    -> selector<float_parameter_id>;
auto make_mixer_channel_pan_balance_parameter_selector(mixer::channel_id)
    -> selector<float_parameter_id>;
auto make_mixer_channel_record_parameter_selector(mixer::channel_id)
    -> selector<bool_parameter_id>;
auto make_mixer_channel_mute_parameter_selector(mixer::channel_id)
    -> selector<bool_parameter_id>;
auto make_mixer_channel_solo_parameter_selector(mixer::channel_id)
    -> selector<bool_parameter_id>;
auto make_mixer_channel_out_stream_selector(mixer::channel_id)
    -> selector<audio_stream_id>;
auto make_aux_send_volume_parameter_selector(
    mixer::channel_id,
    mixer::channel_id aux_id) -> selector<float_parameter_id>;
auto make_aux_send_active_selector(mixer::channel_id, mixer::channel_id aux_id)
    -> selector<bool_parameter_id>;
auto
make_aux_send_fader_tap_selector(mixer::channel_id, mixer::channel_id aux_id)
    -> selector<enum_parameter_id>;
auto
make_can_toggle_aux_send_selector(mixer::channel_id, mixer::channel_id aux_id)
    -> selector<bool>;

auto make_aux_channel_default_fader_tap_parameter_selector(mixer::channel_id)
    -> selector<enum_parameter_id>;

struct mixer_device_route
{
    external_audio::device_id device_id;
    string_id name;

    auto operator==(mixer_device_route const&) const noexcept -> bool = default;
};

struct mixer_channel_route
{
    mixer::channel_id channel_id;
    string_id name;

    auto operator==(mixer_channel_route const&) const noexcept
        -> bool = default;
};

auto make_mixer_channel_mix_input_is_valid_selector(mixer::channel_id)
    -> selector<bool>;

struct selected_route
{
    enum class state_t
    {
        valid,
        not_mixed,
    } state;

    boxed_string name;

    auto operator==(selected_route const&) const noexcept -> bool = default;
};

auto make_mixer_channel_selected_route_selector(mixer::channel_id, io_direction)
    -> selector<selected_route>;

auto make_mixer_device_routes_selector(mixer::channel_type, io_direction)
    -> selector<boxed_vector<mixer_device_route>>;

auto make_mixer_channel_routes_selector(mixer::channel_id, io_direction)
    -> selector<boxed_vector<mixer_channel_route>>;

auto make_mixer_channel_name_selector(mixer::channel_id) -> selector<string_id>;

auto make_mixer_channel_name_string_selector(mixer::channel_id)
    -> selector<boxed_string>;

auto make_mixer_channel_can_move_left_selector(mixer::channel_id)
    -> selector<bool>;
auto make_mixer_channel_can_move_right_selector(mixer::channel_id)
    -> selector<bool>;

extern selector<box<midi::device_ids_t>> const select_midi_input_devices;

auto make_midi_device_name_selector(midi::device_id_t)
    -> selector<boxed_string>;

auto make_midi_device_enabled_selector(midi::device_id_t) -> selector<bool>;

auto make_muted_by_solo_selector(mixer::channel_id) -> selector<bool>;

auto make_fx_chain_selector(mixer::channel_id) -> selector<box<fx::chain_t>>;

auto make_fx_module_instance_id_selector(fx::module_id)
    -> selector<fx::instance_id>;
auto make_fx_module_name_selector(fx::module_id) -> selector<boxed_string>;
auto make_fx_module_bus_type_selector(fx::module_id)
    -> selector<audio::bus_type>;
auto make_fx_module_active_selector(fx::module_id)
    -> selector<bool_parameter_id>;
auto make_fx_module_parameters_selector(fx::module_id)
    -> selector<box<parameters_map>>;
auto make_fx_module_can_move_up_selector(mixer::channel_id) -> selector<bool>;
auto make_fx_module_can_move_down_selector(mixer::channel_id) -> selector<bool>;
auto make_parameter_name_selector(parameter_id) -> selector<boxed_string>;
auto make_parameter_value_string_selector(parameter_id)
    -> selector<std::string>;
auto make_fx_module_streams_selector(fx::module_id)
    -> selector<box<fx::module_streams>>;
auto make_audio_stream_selector(audio_stream_id)
    -> selector<audio_stream_buffer>;

auto make_float_parameter_bipolar_selector(float_parameter_id)
    -> selector<bool>;
auto make_enum_parameter_values_selector(enum_parameter_id)
    -> selector<std::vector<std::pair<std::string, int>>>;

template <class P>
auto make_parameter_min_selector(parameter::id_t<P>)
    -> selector<parameter::value_type_t<P>>;

template <class P>
auto make_parameter_max_selector(parameter::id_t<P>)
    -> selector<parameter::value_type_t<P>>;

template <class P>
auto make_parameter_value_selector(parameter::id_t<P>)
    -> selector<parameter::value_type_t<P>>;

template <class P>
auto make_parameter_normalized_value_selector(parameter::id_t<P>)
    -> selector<float>;

auto make_parameter_is_midi_assignable_selector(parameter_id) -> selector<bool>;

auto make_is_midi_learning_selector(parameter_id) -> selector<bool>;

auto make_midi_assignment_selector(parameter_id)
    -> selector<std::optional<midi_assignment>>;

extern selector<bool> const select_midi_learning;

extern selector<fx::registry> const select_fx_registry;

extern selector<bool> const select_recording;

extern selector<std::size_t> const select_xruns;
extern selector<float> const select_cpu_load;

extern selector<std::size_t> const select_display_rotation;
extern selector<bool> const select_on_screen_keyboard_enabled;

extern selector<root_view_mode> const select_root_view_mode;
extern selector<mixer::channel_id> const select_fx_browser_fx_chain;

extern selector<mixer::channel_id> const select_focused_fx_chain;
extern selector<fx::module_id> const select_focused_fx_module;
extern selector<boxed_string> const select_focused_fx_module_name;
extern selector<material_color> const select_focused_fx_module_color;
extern selector<bool> const select_can_show_prev_fx_module;
extern selector<bool> const select_can_show_next_fx_module;

extern selector<startup_session> const select_startup_session;
extern selector<box<std::filesystem::path>> const select_current_session;
extern selector<bool> const select_session_modified;

} // namespace piejam::runtime::selectors
