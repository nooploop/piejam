// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/actions/persistence_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <filesystem>

namespace piejam::runtime::actions
{

struct load_session final
    : ui::cloneable_action<load_session, action>
    , visitable_persistence_action<load_session>
{
    load_session(std::filesystem::path file)
        : file(std::move(file))
    {
    }

    std::filesystem::path file;
};

} // namespace piejam::runtime::actions
