// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

namespace piejam::audio::ladspa
{

using plugin_id_t = unsigned long;

struct plugin_descriptor;
struct port_descriptor;

class plugin;

} // namespace piejam::audio::ladspa
