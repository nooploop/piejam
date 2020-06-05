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

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

namespace piejam::runtime
{

inline constexpr unsigned current_app_config_version = 0;

struct app_config
{
    std::string input_device_name;
    std::string output_device_name;
    unsigned samplerate{};
    unsigned period_size{};
};

auto to_json(app_config const&) -> nlohmann::json;
auto from_json(nlohmann::json const&) -> app_config;

auto load_app_config(std::filesystem::path const&) -> app_config;
void save_app_config(app_config const&, std::filesystem::path const&);

} // namespace piejam::runtime
