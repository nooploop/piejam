// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/startup_session.h>

#include <piejam/audio/period_size.h>
#include <piejam/audio/sample_rate.h>

#include <nlohmann/json_fwd.hpp>

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

namespace piejam::runtime::persistence
{

inline constexpr unsigned current_app_config_version = 0;

struct app_config
{
    std::string sound_card;
    audio::sample_rate sample_rate{};
    audio::period_size period_size{};

    std::vector<std::string> enabled_midi_input_devices;

    std::size_t rec_session{};

    std::size_t display_rotation{};
    bool on_screen_keyboard_enabled{};

    runtime::startup_session startup_session;
    std::filesystem::path last_session_file;
};

auto load_app_config(std::istream&) -> app_config;
void save_app_config(std::ostream&, app_config const&);

} // namespace piejam::runtime::persistence
