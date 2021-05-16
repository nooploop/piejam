// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/app_config.h>

#include <nlohmann/json.hpp>

#include <istream>
#include <ostream>

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

namespace piejam::runtime::persistence
{

using namespace std::string_literals;

static auto const s_key_version = "version";
static auto const s_key_audio_settings = "audio_settings";
static auto const s_key_input_device_name = "input_device_name"s;
static auto const s_key_output_device_name = "output_device_name";
static auto const s_key_sample_rate = "sample_rate";
static auto const s_key_period_size = "period_size";
static auto const s_key_period_count = "period_count";
static auto const s_key_input_configs = "input_configs";
static auto const s_key_output_configs = "output_configs";
static auto const s_key_enabled_midi_input_devices =
        "enabled_midi_input_devices";

static auto
get_version(nlohmann::json const& conf) -> unsigned
{
    return conf.at(s_key_version).get<unsigned>();
}

void
to_json(nlohmann::json& j, bus_config const& conf)
{
    j = nlohmann::json{
            {"name", conf.name},
            {"bus_type", conf.bus_type},
            {"channels", conf.channels}};
}

void
from_json(nlohmann::json const& j, bus_config& conf)
{
    j.at("name").get_to(conf.name);
    j.at("bus_type").get_to(conf.bus_type);
    j.at("channels").get_to(conf.channels);
}

void
to_json(nlohmann::json& j, app_config const& conf)
{
    j = {{s_key_version, current_app_config_version},
         {s_key_audio_settings,
          {{s_key_input_device_name, conf.input_device_name},
           {s_key_output_device_name, conf.output_device_name},
           {s_key_sample_rate, conf.sample_rate},
           {s_key_period_size, conf.period_size},
           {s_key_period_count, conf.period_count},
           {s_key_input_configs, conf.input_bus_config},
           {s_key_output_configs, conf.output_bus_config},
           {s_key_enabled_midi_input_devices,
            conf.enabled_midi_input_devices}}}};
}

void
from_json(nlohmann::json const& j, app_config& conf)
{
    auto const& audio_settings = j.at(s_key_audio_settings);
    audio_settings.at(s_key_input_device_name).get_to(conf.input_device_name);
    audio_settings.at(s_key_output_device_name).get_to(conf.output_device_name);
    audio_settings.at(s_key_sample_rate).get_to(conf.sample_rate);
    audio_settings.at(s_key_period_size).get_to(conf.period_size);
    audio_settings.at(s_key_period_count).get_to(conf.period_count);
    audio_settings.at(s_key_input_configs).get_to(conf.input_bus_config);
    audio_settings.at(s_key_output_configs).get_to(conf.output_bus_config);
    audio_settings.at(s_key_enabled_midi_input_devices)
            .get_to(conf.enabled_midi_input_devices);
}

using upgrade_function = void (*)(nlohmann::json&);
using upgrade_functions_array =
        std::array<upgrade_function, current_app_config_version>;

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
            std::make_index_sequence<current_app_config_version>());
}

static void
upgrade_app_config(nlohmann::json& conf)
{
    static auto const upgrade_functions = make_upgrade_functions_array();

    auto file_version = get_version(conf);
    while (file_version < current_app_config_version)
    {
        upgrade_functions[file_version](conf);

        auto const prev_version = file_version;
        file_version = get_version(conf);

        assert(file_version > prev_version);
        if (file_version <= prev_version)
            throw std::runtime_error("cannot upgrade config file");
    }
}

auto
load_app_config(std::istream& in) -> app_config
{
    auto json_conf = nlohmann::json::parse(in);

    auto const file_version = get_version(json_conf);

    if (file_version > current_app_config_version)
        throw std::runtime_error("app config version is too new");

    if (file_version < current_app_config_version)
        upgrade_app_config(json_conf);

    assert(current_app_config_version == get_version(json_conf));

    app_config conf;
    json_conf.get_to(conf);
    return conf;
}

void
save_app_config(std::ostream& out, app_config const& conf)
{
    out << nlohmann::json(conf).dump(4) << std::endl;
}

} // namespace piejam::runtime::persistence
