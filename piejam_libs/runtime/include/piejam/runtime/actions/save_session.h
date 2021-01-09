// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/ladspa/fwd.h>
#include <piejam/entity_id_hash.h>
#include <piejam/runtime/actions/persistence_action.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/cloneable_action.h>

#include <filesystem>
#include <unordered_map>

namespace piejam::runtime::actions
{

struct save_session final
    : ui::cloneable_action<save_session, action>
    , visitable_persistence_action<save_session>
{
    save_session(std::filesystem::path file)
        : file(std::move(file))
    {
    }

    std::filesystem::path file;

    auto reduce(state const&) const -> state override;
};

} // namespace piejam::runtime::actions
