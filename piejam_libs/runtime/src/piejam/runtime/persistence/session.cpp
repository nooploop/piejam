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

#include <piejam/runtime/persistence/session.h>

#include <piejam/functional/overload.h>
#include <piejam/runtime/fx/internal.h>

#include <nlohmann/json.hpp>

#include <boost/assert.hpp>

#include <fstream>

namespace piejam::runtime
{

namespace fx
{

NLOHMANN_JSON_SERIALIZE_ENUM(internal, {{internal::gain, "gain"}})

} // namespace fx

namespace persistence
{

static auto const s_key_version = "version";

static auto
get_version(nlohmann::json const& json_ses) -> unsigned
{
    return json_ses.at(s_key_version).get<unsigned>();
}

static auto const s_key_internal = "internal";
static auto const s_key_ladspa = "ladspa";

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(session::ladspa_plugin, id, name)

void
to_json(nlohmann::json& j, session::fx_plugin const& fx_plug)
{
    std::visit(
            overload{
                    [&j](fx::internal id) {
                        j = {{s_key_internal, id}};
                    },
                    [&j](session::ladspa_plugin const& ladspa_plug) {
                        j = {{s_key_ladspa, ladspa_plug}};
                    }},
            fx_plug.as_variant());
}

void
from_json(nlohmann::json const& j, session::fx_plugin& fx_plug)
{
    if (j.contains(s_key_internal))
    {
        fx::internal id;
        j[s_key_internal].get_to<fx::internal>(id);
        fx_plug = id;
    }
    else if (j.contains(s_key_ladspa))
    {
        session::ladspa_plugin ladspa_plug;
        j[s_key_ladspa].get_to<session::ladspa_plugin>(ladspa_plug);
        fx_plug = ladspa_plug;
    }
    else
    {
        throw std::runtime_error("unknown fx_plugin_id");
    }
}

static auto const s_key_inputs = "inputs";
static auto const s_key_outputs = "outputs";

void
to_json(nlohmann::json& json_ses, session const& ses)
{
    json_ses = {
            {s_key_version, current_session_version},
            {s_key_inputs, ses.inputs},
            {s_key_outputs, ses.outputs}};
}

void
from_json(nlohmann::json const& json_ses, session& ses)
{
    json_ses.at(s_key_inputs)
            .get_to<std::vector<session::fx_chain>>(ses.inputs);
    json_ses.at(s_key_outputs)
            .get_to<std::vector<session::fx_chain>>(ses.outputs);
}

using upgrade_function = void (*)(nlohmann::json&);
using upgrade_functions_array =
        std::array<upgrade_function, current_session_version>;

template <size_t Version>
static void upgrade(nlohmann::json&);

template <size_t... I>
static auto make_upgrade_functions_array(std::index_sequence<I...>)
        -> upgrade_functions_array
{
    return upgrade_functions_array{{upgrade<I>...}};
}

static auto
make_upgrade_functions_array() -> upgrade_functions_array
{
    return make_upgrade_functions_array(
            std::make_index_sequence<current_session_version>());
}

static void
upgrade_session(nlohmann::json& json_ses)
{
    static auto const upgrade_functions = make_upgrade_functions_array();

    auto file_version = get_version(json_ses);
    while (file_version < current_session_version)
    {
        upgrade_functions[file_version](json_ses);

        auto const prev_version = file_version;
        file_version = get_version(json_ses);

        BOOST_ASSERT(file_version > prev_version);
        if (file_version <= prev_version)
            throw std::runtime_error("cannot upgrade session file");
    }
}

auto
load_session(std::istream& in) -> session
{
    auto json_ses = nlohmann::json::parse(in);

    auto const file_version = get_version(json_ses);

    if (file_version > current_session_version)
        throw std::runtime_error("session version is too new");

    if (file_version < current_session_version)
        upgrade_session(json_ses);

    BOOST_ASSERT(current_session_version == get_version(json_ses));

    session ses;
    json_ses.get_to(ses);
    return ses;
}

void
save_session(std::ostream& out, session const& ses)
{
    out << nlohmann::json(ses).dump(4) << std::endl;
}

} // namespace persistence

} // namespace piejam::runtime
