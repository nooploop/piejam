// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/types.h>
#include <piejam/runtime/channel_index_pair.h>

#include <nlohmann/json_fwd.hpp>

#include <iosfwd>
#include <string>
#include <vector>

namespace piejam::runtime::persistence
{

inline constexpr unsigned current_app_config_version = 0;

struct bus_config
{
    std::string name;
    audio::bus_type bus_type;
    channel_index_pair channels;
};

struct app_config
{
    std::string input_device_name;
    std::string output_device_name;
    unsigned samplerate{};
    unsigned period_size{};

    std::vector<bus_config> input_bus_config;
    std::vector<bus_config> output_bus_config;

    std::vector<std::string> enabled_midi_input_devices;
};

void to_json(nlohmann::json&, app_config const&);
void from_json(nlohmann::json const&, app_config&);

auto load_app_config(std::istream&) -> app_config;
void save_app_config(std::ostream&, app_config const&);

} // namespace piejam::runtime::persistence
