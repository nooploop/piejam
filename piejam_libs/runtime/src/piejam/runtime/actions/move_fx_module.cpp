// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/move_fx_module.h>

#include <piejam/runtime/state.h>

#include <algorithm>

namespace piejam::runtime::actions
{

void
move_fx_module_up::reduce(state& st) const
{
    auto channels = st.mixer_state.channels.lock();
    auto& mixer_channel = channels[st.gui_state.focused_fx_chain_id];

    auto fx_chain = mixer_channel.fx_chain.lock();

    if (auto it = std::ranges::find(*fx_chain, st.gui_state.focused_fx_mod_id);
        it != fx_chain->end() && it != fx_chain->begin())
    {
        std::iter_swap(it, std::prev(it));
    }
}

void
move_fx_module_down::reduce(state& st) const
{
    auto channels = st.mixer_state.channels.lock();
    auto& mixer_channel = channels[st.gui_state.focused_fx_chain_id];

    auto fx_chain = mixer_channel.fx_chain.lock();

    if (auto it = std::ranges::find(*fx_chain, st.gui_state.focused_fx_mod_id);
        it != fx_chain->end() && std::next(it) != fx_chain->end())
    {
        std::iter_swap(it, std::next(it));
    }
}

} // namespace piejam::runtime::actions
