// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <span>

namespace piejam::runtime::fx
{

class ladspa_control
{
public:
    virtual ~ladspa_control() = default;

    virtual auto load(ladspa::plugin_descriptor const&)
            -> ladspa_instance_id = 0;
    virtual void unload(ladspa_instance_id const&) = 0;

    virtual auto control_inputs(ladspa_instance_id const&) const
            -> std::span<ladspa::port_descriptor const> = 0;
};

} // namespace piejam::runtime::fx
