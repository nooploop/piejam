// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/midi/device_id.h>
#include <piejam/runtime/actions/audio_io_process_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

namespace piejam::runtime::actions
{

struct deactivate_midi_device final
    : ui::cloneable_action<deactivate_midi_device, reducible_action>
    , visitable_audio_io_process_action<deactivate_midi_device>
{
    midi::device_id_t device_id;

    void reduce(state&) const override;
};

} // namespace piejam::runtime::actions
