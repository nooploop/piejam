// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::ladspa
{

struct plugin_descriptor
{
    plugin_id_t id{};
    std::filesystem::path file{};
    unsigned long index{};
    std::string label{};
    std::string name{};
    std::string author{};
    std::string copyright{};
    std::size_t num_inputs{};
    std::size_t num_outputs{};
};

[[nodiscard]]
inline constexpr auto
operator==(plugin_descriptor const& l, plugin_descriptor const& r) -> bool
{
    return l.id == r.id;
}

[[nodiscard]]
inline constexpr auto
operator!=(plugin_descriptor const& l, plugin_descriptor const& r) -> bool
{
    return l.id != r.id;
}

} // namespace piejam::ladspa
