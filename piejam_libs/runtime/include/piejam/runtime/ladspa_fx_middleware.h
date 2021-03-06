// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/middleware_functors.h>

namespace piejam::runtime
{

class ladspa_fx_middleware final : private middleware_functors
{
public:
    ladspa_fx_middleware(middleware_functors, fx::ladspa_control&);

    void operator()(action const&);

private:
    template <class Action>
    void process_ladspa_fx_action(Action const&);

    fx::ladspa_control& m_ladspa_control;
};

} // namespace piejam::runtime
