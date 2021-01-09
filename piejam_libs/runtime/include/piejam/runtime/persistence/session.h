// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/persistence/fx_preset.h>

#include <nlohmann/json.hpp>

#include <iosfwd>
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
    };

    struct ladspa_plugin
    {
        audio::ladspa::plugin_id_t id{};
        std::string name;
        fx_preset preset;
    };

    struct fx_plugin : std::variant<internal_fx, ladspa_plugin>
    {
        using base_t = std::variant<internal_fx, ladspa_plugin>;
        using base_t::variant;
        auto as_variant() const noexcept -> base_t const& { return *this; }
    };

    using fx_chain = std::vector<fx_plugin>;

    std::vector<fx_chain> inputs;
    std::vector<fx_chain> outputs;
};

void to_json(nlohmann::json&, session const&);
void from_json(nlohmann::json const&, session&);

auto load_session(std::istream&) -> session;
void save_session(std::ostream&, session const&);

} // namespace piejam::runtime::persistence
