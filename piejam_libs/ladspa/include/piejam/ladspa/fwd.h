// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/fwd.h>

namespace piejam::ladspa
{

using plugin_id_t = unsigned long;

struct plugin_descriptor;
struct port_descriptor;

class plugin;

struct instance_id_tag;
using instance_id = entity_id<instance_id_tag>;

class instance_manager;
class processor_factory;

} // namespace piejam::ladspa
