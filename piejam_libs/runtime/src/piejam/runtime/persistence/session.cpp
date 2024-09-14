// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/session.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/persistence/fx_internal_id.h>

#include <nlohmann/json.hpp>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::audio
{

NLOHMANN_JSON_SERIALIZE_ENUM(
        bus_type,
        {{bus_type::mono, "mono"}, {bus_type::stereo, "stereo"}})

} // namespace piejam::audio

namespace piejam::runtime
{

NLOHMANN_JSON_SERIALIZE_ENUM(
        midi_assignment::type,
        {{midi_assignment::type::cc, "cc"}})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        midi_assignment,
        channel,
        control_type,
        control_id);

} // namespace piejam::runtime

namespace piejam::runtime::fx
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(parameter_midi_assignment, key, value);

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

static auto const s_key_version = "version";

static auto
get_version(nlohmann::json const& json_ses) -> unsigned
{
    return json_ses.at(s_key_version).get<unsigned>();
}

static auto const s_key_internal = "internal";
static auto const s_key_ladspa = "ladspa";

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(session::internal_fx, type, preset, midi);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::ladspa_plugin,
        id,
        name,
        preset,
        midi);

void
to_json(nlohmann::json& j, session::fx_plugin const& fx_plug)
{
    std::visit(
            boost::hof::match(
                    [&j](session::internal_fx const& fx) {
                        j = {{s_key_internal, fx}};
                    },
                    [&j](session::ladspa_plugin const& ladspa_plug) {
                        j = {{s_key_ladspa, ladspa_plug}};
                    },
                    [](std::monostate) { BOOST_ASSERT(false); }),
            fx_plug.as_variant());
}

void
from_json(nlohmann::json const& j, session::fx_plugin& fx_plug)
{
    if (j.contains(s_key_internal))
    {
        session::internal_fx fx;
        j[s_key_internal].get_to(fx);
        fx_plug = fx;
    }
    else if (j.contains(s_key_ladspa))
    {
        session::ladspa_plugin ladspa_plug;
        j[s_key_ladspa].get_to(ladspa_plug);
        fx_plug = ladspa_plug;
    }
    else
    {
        throw std::runtime_error("unknown fx_plugin_id");
    }
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::mixer_parameters,
        volume,
        pan,
        mute);

void
to_json(nlohmann::json& j, session::mixer_midi const& midi)
{
    if (midi.volume)
    {
        j["volume"] = *midi.volume;
    }

    if (midi.pan)
    {
        j["pan"] = *midi.pan;
    }

    if (midi.mute)
    {
        j["mute"] = *midi.mute;
    }
}

void
from_json(nlohmann::json const& j, session::mixer_midi& midi)
{
    if (j.contains("volume"))
    {
        midi.volume = j.at("volume").get<midi_assignment>();
    }

    if (j.contains("pan"))
    {
        midi.pan = j.at("pan").get<midi_assignment>();
    }

    if (j.contains("mute"))
    {
        midi.mute = j.at("mute").get<midi_assignment>();
    }
}

NLOHMANN_JSON_SERIALIZE_ENUM(
        session::mixer_io_type,
        {{session::mixer_io_type::default_, "default"},
         {session::mixer_io_type::device, "device"},
         {session::mixer_io_type::channel, "channel"}})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(session::mixer_io, type, name);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::mixer_channel,
        name,
        bus_type,
        parameter,
        midi,
        fx_chain,
        in,
        out);

static auto const s_key_mixer_channels = "mixer_channels";
static auto const s_key_main_mixer_channel = "main_mixer_channel";

void
to_json(nlohmann::json& json_ses, session const& ses)
{
    json_ses = {
            {s_key_version, current_session_version},
            {s_key_mixer_channels, ses.mixer_channels},
            {s_key_main_mixer_channel, ses.main_mixer_channel}};
}

void
from_json(nlohmann::json const& json_ses, session& ses)
{
    json_ses.at(s_key_mixer_channels).get_to(ses.mixer_channels);
    json_ses.at(s_key_main_mixer_channel).get_to(ses.main_mixer_channel);
}

using upgrade_function = void (*)(nlohmann::json&);
using upgrade_functions_array =
        std::array<upgrade_function, current_session_version>;

template <size_t Version>
static void upgrade(nlohmann::json&);

template <size_t... I>
static auto
make_upgrade_functions_array(std::index_sequence<I...>)
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
        {
            throw std::runtime_error("cannot upgrade session file");
        }
    }
}

auto
load_session(std::istream& in) -> session
{
    auto json_ses = nlohmann::json::parse(in);

    auto const file_version = get_version(json_ses);

    if (file_version > current_session_version)
    {
        throw std::runtime_error("session version is too new");
    }

    if (file_version < current_session_version)
    {
        upgrade_session(json_ses);
    }

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

session::~session() = default;

} // namespace piejam::runtime::persistence
