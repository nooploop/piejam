// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>

#include <span>

namespace piejam::ladspa
{

class instance_manager
{
public:
    virtual ~instance_manager() = default;

    virtual auto load(plugin_descriptor const&) -> instance_id = 0;
    virtual void unload(instance_id const&) = 0;

    virtual auto control_inputs(instance_id const&) const
            -> std::span<port_descriptor const> = 0;
};

} // namespace piejam::ladspa
