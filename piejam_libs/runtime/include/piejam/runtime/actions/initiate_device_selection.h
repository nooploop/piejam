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

struct initiate_device_selection final
    : ui::cloneable_action<initiate_device_selection, action>
    , visitable_audio_io_process_action<initiate_device_selection>
{
    io_direction io_dir{};
    std::size_t index{};
};

} // namespace piejam::runtime::actions
