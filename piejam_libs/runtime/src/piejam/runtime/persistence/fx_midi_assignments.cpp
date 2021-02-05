// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/fx_preset.h>

#include <piejam/runtime/fx/parameter_assignment.h>
#include <piejam/runtime/midi_assignment.h>
#include <piejam/runtime/persistence/midi_assignment.h>

namespace piejam::runtime::fx
{

void
to_json(nlohmann::json& j, parameter_midi_assignment const& p)
{
    j = {{"key", p.key}, {"value", p.value}};
}

void
from_json(nlohmann::json const& j, parameter_midi_assignment& p)
{
    j.at("key").get_to(p.key);
    j.at("value").get_to(p.value);
}

} // namespace piejam::runtime::fx
