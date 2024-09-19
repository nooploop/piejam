// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/period_count.h>
#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>

#include <nlohmann/json_fwd.hpp>

#include <iosfwd>
#include <string>
#include <vector>

namespace piejam::runtime::persistence
{

inline constexpr unsigned current_app_config_version = 0;

struct app_config
{
    std::string input_sound_card;
    std::string output_sound_card;
    audio::sample_rate sample_rate{};
    audio::period_size period_size{};
    audio::period_count period_count{};

    std::vector<std::string> enabled_midi_input_devices;

    std::size_t rec_session{};
};

void to_json(nlohmann::json&, app_config const&);
void from_json(nlohmann::json const&, app_config&);

auto load_app_config(std::istream&) -> app_config;
void save_app_config(std::ostream&, app_config const&);

} // namespace piejam::runtime::persistence
