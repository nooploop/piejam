// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/midi_control_action.h>
#include <piejam/runtime/actions/persistence_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <filesystem>
#include <string>
#include <vector>

namespace piejam::runtime::actions
{

struct save_app_config final
    : ui::cloneable_action<save_app_config, action>
    , visitable_persistence_action<save_app_config>
    , visitable_midi_control_action<save_app_config>
{
    save_app_config(std::filesystem::path file)
        : file(std::move(file))
    {
    }

    std::filesystem::path file;
    std::vector<std::string> enabled_midi_devices;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
