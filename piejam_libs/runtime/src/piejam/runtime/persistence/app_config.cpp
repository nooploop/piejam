// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/app_config.h>

#include <piejam/runtime/persistence/strong_type.h>

#include <nlohmann/json.hpp>

#include <istream>
#include <ostream>

namespace piejam::audio
{

M_PIEJAM_PERSISTENCE_DEFINE_STRONG_TYPE_SERIALIER(
        sample_rate,
        unsigned,
        "sample_rate");

M_PIEJAM_PERSISTENCE_DEFINE_STRONG_TYPE_SERIALIER(
        period_size,
        unsigned,
        "period_size");

M_PIEJAM_PERSISTENCE_DEFINE_STRONG_TYPE_SERIALIER(
        period_count,
        unsigned,
        "period_count");

} // namespace piejam::audio

namespace piejam::runtime::persistence
{

static auto const s_key_version = "version";
static auto const s_key_app_config = "app_config";

static auto
get_version(nlohmann::json const& conf) -> unsigned
{
    return conf.at(s_key_version).get<unsigned>();
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        app_config,
        input_sound_card,
        output_sound_card,
        sample_rate,
        period_size,
        period_count,
        enabled_midi_input_devices,
        rec_session);

using upgrade_function = void (*)(nlohmann::json&);
using upgrade_functions_array =
        std::array<upgrade_function, current_app_config_version>;

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
        {
            throw std::runtime_error("cannot upgrade config file");
        }
    }
}

auto
load_app_config(std::istream& in) -> app_config
{
    auto json_conf = nlohmann::json::parse(in);

    auto const file_version = get_version(json_conf);

    if (file_version > current_app_config_version)
    {
        throw std::runtime_error("app config version is too new");
    }

    if (file_version < current_app_config_version)
    {
        upgrade_app_config(json_conf);
    }

    assert(current_app_config_version == get_version(json_conf));

    app_config conf;
    json_conf.at(s_key_app_config).get_to(conf);
    return conf;
}

void
save_app_config(std::ostream& out, app_config const& conf)
{
    nlohmann::json const json_conf = {
            {s_key_version, current_app_config_version},
            {s_key_app_config, conf}};
    out << json_conf.dump(4) << '\n';
}

} // namespace piejam::runtime::persistence
