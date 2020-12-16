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

#include <nlohmann/json_fwd.hpp>

#include <iosfwd>
#include <variant>
#include <vector>

namespace piejam::runtime::persistence
{

inline constexpr unsigned current_session_version = 0;

struct fx_plugin_id : std::variant<fx::internal, audio::ladspa::plugin_id_t>
{
    using base_t = std::variant<fx::internal, audio::ladspa::plugin_id_t>;
    using base_t::variant;
    auto as_variant() const noexcept -> base_t const& { return *this; }
};

struct session
{
    using fx_chain_data = std::vector<fx_plugin_id>;

    std::vector<fx_chain_data> inputs;
    std::vector<fx_chain_data> outputs;
};

void to_json(nlohmann::json&, session const&);
void from_json(nlohmann::json const&, session&);

auto load_session(std::istream&) -> session;
void save_session(std::ostream&, session const&);

} // namespace piejam::runtime::persistence
