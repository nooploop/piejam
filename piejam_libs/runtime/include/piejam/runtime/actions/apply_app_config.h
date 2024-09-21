// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/audio_io_process_action.h>
#include <piejam/runtime/actions/midi_control_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/persistence/app_config.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct apply_app_config final
    : ui::cloneable_action<apply_app_config, reducible_action>
    , visitable_audio_io_process_action<apply_app_config>
    , visitable_midi_control_action<apply_app_config>
{
    persistence::app_config conf;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
