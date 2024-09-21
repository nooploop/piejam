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

struct select_period_size final
    : ui::cloneable_action<select_period_size, action>
    , visitable_audio_io_process_action<select_period_size>
{
    std::size_t index{};
};

} // namespace piejam::runtime::actions
