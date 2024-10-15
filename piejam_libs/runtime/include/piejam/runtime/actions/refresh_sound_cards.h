// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/audio_io_process_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct refresh_sound_cards final
    : ui::cloneable_action<refresh_sound_cards, action>
    , visitable_audio_io_process_action<refresh_sound_cards>
{
};

} // namespace piejam::runtime::actions
