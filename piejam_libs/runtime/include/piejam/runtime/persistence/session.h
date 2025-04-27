// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/channel_index_pair.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/material_color.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameter/assignment.h>
#include <piejam/runtime/persistence/fx_midi_assignments.h>
#include <piejam/runtime/persistence/fx_preset.h>

#include <piejam/audio/types.h>
#include <piejam/ladspa/fwd.h>

#include <nlohmann/json_fwd.hpp>

#include <iosfwd>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace piejam::runtime::persistence
{

inline constexpr unsigned current_session_version = 0;

struct session
{
    struct external_audio_device_config
    {
        std::string name;
        audio::bus_type bus_type;
        channel_index_pair channels;
    };

    struct internal_fx
    {
        fx::internal_id type{};
        fx_preset preset;
        fx_midi_assignments midi;
    };

    struct ladspa_plugin
    {
        ladspa::plugin_id_t id{};
        std::string name;
        fx_preset preset;
        fx_midi_assignments midi;
    };

    struct fx_plugin : std::variant<std::monostate, internal_fx, ladspa_plugin>
    {
        using base_t = std::variant<std::monostate, internal_fx, ladspa_plugin>;
        using base_t::base_t;
        using base_t::operator=;

        [[nodiscard]]
        auto as_variant() const noexcept -> base_t const&
        {
            return *this;
        }
    };

    struct mixer_parameters
    {
        float volume;
        float pan;
        bool mute;
    };

    struct mixer_midi
    {
        std::optional<midi_assignment> volume;
        std::optional<midi_assignment> pan;
        std::optional<midi_assignment> mute;
    };

    using fx_chain_t = std::vector<fx_plugin>;

    enum class mixer_io_type
    {
        default_,
        invalid,
        device,
        channel
    };

    struct mixer_io
    {
        mixer_io_type type;
        std::size_t index;
    };

    struct mixer_aux_send
    {
        mixer_io route;
        bool enabled;
        float volume;
    };

    struct mixer_channel
    {
        std::string name;
        material_color color;
        audio::bus_type bus_type;
        mixer_parameters parameter;
        mixer_midi midi;
        fx_chain_t fx_chain;
        mixer_io in;
        mixer_io out;
        std::vector<mixer_aux_send> aux_sends;
    };

    std::vector<external_audio_device_config> external_audio_input_devices;
    std::vector<external_audio_device_config> external_audio_output_devices;
    mixer_channel main_mixer_channel;
    std::vector<mixer_channel> mixer_channels;
};

auto load_session(std::istream&) -> session;
void save_session(std::ostream&, session const&);

} // namespace piejam::runtime::persistence
