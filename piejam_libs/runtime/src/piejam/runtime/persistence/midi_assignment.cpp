// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/fx_preset.h>

#include <piejam/runtime/midi_assignment.h>

namespace piejam::runtime
{

NLOHMANN_JSON_SERIALIZE_ENUM(
        midi_assignment::type,
        {{midi_assignment::type::cc, "cc"}, {midi_assignment::type::pc, "pc"}})

void
to_json(nlohmann::json& j, midi_assignment const& ass)
{
    j = {{"channel", ass.channel},
         {"type", ass.control_type},
         {"id", ass.control_id}};
}

void
from_json(nlohmann::json const& j, midi_assignment& ass)
{
    j.at("channel").get_to(ass.channel);
    j.at("type").get_to(ass.control_type);
    j.at("id").get_to(ass.control_id);
}

} // namespace piejam::runtime
