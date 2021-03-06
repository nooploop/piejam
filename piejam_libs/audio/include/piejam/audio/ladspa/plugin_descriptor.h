// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::audio::ladspa
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

inline bool
operator==(plugin_descriptor const& l, plugin_descriptor const& r)
{
    return l.id == r.id;
}

inline bool
operator!=(plugin_descriptor const& l, plugin_descriptor const& r)
{
    return l.id != r.id;
}

} // namespace piejam::audio::ladspa
