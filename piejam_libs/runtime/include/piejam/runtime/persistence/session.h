// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/persistence/fx_midi_assignments.h>
#include <piejam/runtime/persistence/fx_preset.h>

#include <nlohmann/json.hpp>

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
    struct internal_fx
    {
        fx::internal type;
        fx_preset preset;
        fx_midi_assignments midi;
    };

    struct ladspa_plugin
    {
        audio::ladspa::plugin_id_t id{};
        std::string name;
        fx_preset preset;
        fx_midi_assignments midi;
    };

    struct fx_plugin : std::variant<internal_fx, ladspa_plugin>
    {
        using base_t = std::variant<internal_fx, ladspa_plugin>;
        using base_t::variant;
        auto as_variant() const noexcept -> base_t const& { return *this; }
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
        device,
        channel
    };

    struct mixer_io
    {
        mixer_io_type type;
        std::string name;
    };

    struct mixer_channel
    {
        std::string name;
        mixer_parameters parameter;
        mixer_midi midi;
        fx_chain_t fx_chain;
        mixer_io in;
        mixer_io out;
    };

    std::vector<mixer_channel> mixer_channels;
    mixer_channel main_mixer_channel;
};

void to_json(nlohmann::json&, session const&);
void from_json(nlohmann::json const&, session&);

auto load_session(std::istream&) -> session;
void save_session(std::ostream&, session const&);

} // namespace piejam::runtime::persistence
