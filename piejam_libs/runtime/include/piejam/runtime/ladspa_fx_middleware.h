// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/ladspa/fwd.h>
#include <piejam/runtime/fwd.h>

namespace piejam::runtime
{

class ladspa_fx_middleware final
{
public:
    ladspa_fx_middleware(ladspa::instance_manager&);

    void operator()(middleware_functors const&, action const&);

private:
    template <class Action>
    void process_ladspa_fx_action(middleware_functors const&, Action const&);

    ladspa::instance_manager& m_ladspa_control;
};

} // namespace piejam::runtime
