// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

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
    struct ladspa_plugin
    {
        audio::ladspa::plugin_id_t id{};
        std::string name;
    };

    struct fx_plugin : std::variant<fx::internal, ladspa_plugin>
    {
        using base_t = std::variant<fx::internal, ladspa_plugin>;
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
