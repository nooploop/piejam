// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/ui/action_visitor.h>

namespace piejam::runtime::actions
{

struct audio_io_process_action_visitor
    : ui::action_visitor_interface<
              apply_app_config,
              refresh_sound_cards,
              initiate_sound_card_selection,
              select_sample_rate,
              select_period_size,
              select_period_count,
              activate_midi_device,
              deactivate_midi_device>
{
};

struct audio_io_process_action
    : ui::visitable_action_interface<audio_io_process_action_visitor>
{
};

template <class Action>
using visitable_audio_io_process_action =
        ui::visitable_action<Action, audio_io_process_action>;

} // namespace piejam::runtime::actions
