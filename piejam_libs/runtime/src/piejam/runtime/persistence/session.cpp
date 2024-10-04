// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/session.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/persistence/fx_internal_id.h>
#include <piejam/runtime/persistence/optional.h>
#include <piejam/runtime/persistence/variant.h>

#include <nlohmann/json.hpp>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

namespace piejam::audio
{

NLOHMANN_JSON_SERIALIZE_ENUM(
        bus_type,
        {{bus_type::mono, "mono"}, {bus_type::stereo, "stereo"}})

template <class T>
void
to_json(nlohmann::json& j, pair<T> const& p)
{
    j = nlohmann::json{{"left", p.left}, {"right", p.right}};
}

template <class T>
void
from_json(nlohmann::json const& j, pair<T>& p)
{
    j.at("left").get_to(p.left);
    j.at("right").get_to(p.right);
}

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

NLOHMANN_JSON_SERIALIZE_ENUM(
        material_color,
        {
                {material_color::amber, "amber"},
                {material_color::blue, "blue"},
                {material_color::blue_grey, "blue_grey"},
                {material_color::brown, "brown"},
                {material_color::cyan, "cyan"},
                {material_color::deep_orange, "deep_orange"},
                {material_color::deep_purple, "deep_purple"},
                {material_color::green, "green"},
                {material_color::grey, "grey"},
                {material_color::indigo, "indigo"},
                {material_color::light_blue, "light_blue"},
                {material_color::light_green, "light_green"},
                {material_color::lime, "lime"},
                {material_color::orange, "orange"},
                {material_color::pink, "pink"},
                {material_color::red, "red"},
                {material_color::teal, "teal"},
                {material_color::yellow, "yellow"},
        });

} // namespace piejam::runtime

namespace piejam::runtime::fx
{

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(parameter_midi_assignment, key, value);

auto const parameter_value_assignment_serializer =
        persistence::variant_serializer{
                persistence::variant_option<float>{"float"},
                persistence::variant_option<int>{"int"},
                persistence::variant_option<bool>{"bool"},
        };

void
to_json(nlohmann::json& j, parameter_value_assignment const& p)
{
    j = {{"key", p.key},
         {"value", parameter_value_assignment_serializer.to_json(p.value)}};
}

void
from_json(nlohmann::json const& j, parameter_value_assignment& p)
{
    j.at("key").get_to(p.key);

    auto const& v = j.at("value");
    parameter_value_assignment_serializer.from_json(v, p.value);
}

} // namespace piejam::runtime::fx

namespace piejam::runtime::persistence
{

static auto const s_key_version = "version";
static auto const s_key_session = "session";

static auto
get_version(nlohmann::json const& json_ses) -> unsigned
{
    return json_ses.at(s_key_version).get<unsigned>();
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::external_audio_device_config,
        name,
        bus_type,
        channels);

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

static optional_serializer<midi_assignment> const s_midi_volume{"volume"};
static optional_serializer<midi_assignment> const s_midi_pan{"pan"};
static optional_serializer<midi_assignment> const s_midi_mute{"mute"};

void
to_json(nlohmann::json& j, session::mixer_midi const& midi)
{
    s_midi_volume.to_json(j, midi.volume);
    s_midi_pan.to_json(j, midi.pan);
    s_midi_mute.to_json(j, midi.mute);
}

void
from_json(nlohmann::json const& j, session::mixer_midi& midi)
{
    s_midi_volume.from_json(j, midi.volume);
    s_midi_pan.from_json(j, midi.pan);
    s_midi_mute.from_json(j, midi.mute);
}

NLOHMANN_JSON_SERIALIZE_ENUM(
        session::mixer_io_type,
        {{session::mixer_io_type::default_, "default"},
         {session::mixer_io_type::device, "device"},
         {session::mixer_io_type::channel, "channel"}})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(session::mixer_io, type, index, name);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::mixer_aux_send,
        route,
        enabled,
        volume);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session::mixer_channel,
        name,
        color,
        bus_type,
        parameter,
        midi,
        fx_chain,
        in,
        out,
        aux_sends);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        session,
        external_audio_input_devices,
        external_audio_output_devices,
        main_mixer_channel,
        mixer_channels);

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
    json_ses.at(s_key_session).get_to(ses);
    return ses;
}

void
save_session(std::ostream& out, session const& ses)
{
    nlohmann::json json_ses = {
            {s_key_version, current_session_version},
            {s_key_session, ses}};
    out << json_ses.dump(4) << std::endl;
}

} // namespace piejam::runtime::persistence
