// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxBrowserEntry.h>

#include <piejam/entity_id.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

auto
FxBrowserEntry::chainIdFromIndex(unsigned const chainIndex)
        -> runtime::mixer::channel_id
{
    return chainIndex == 0
                   ? observe_once(runtime::selectors::select_mixer_main_channel)
                   : observe_once(
                             runtime::selectors::select_mixer_input_channels)
                             ->at(chainIndex - 1);
}

} // namespace piejam::gui::model
