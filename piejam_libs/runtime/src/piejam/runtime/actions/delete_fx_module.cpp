// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/delete_fx_module.h>

#include <piejam/algorithm/index_of.h>

#include <piejam/runtime/state.h>

namespace piejam::runtime::actions
{

void
delete_fx_module::reduce(state& st) const
{
    if (st.gui_state.focused_fx_mod_id == fx_mod_id)
    {
        auto& mixer_channel = st.mixer_state.channels[fx_chain_id];

        auto next_focused_fx_mod_id = [&]() -> fx::module_id {
            BOOST_ASSERT(!mixer_channel.fx_chain->empty());
            if (mixer_channel.fx_chain->size() == 1)
            {
                return {};
            }
            else
            {
                auto fx_mod_it =
                        std::ranges::find(*mixer_channel.fx_chain, fx_mod_id);
                auto next_fx_mod_it = std::next(fx_mod_it);
                return next_fx_mod_it == mixer_channel.fx_chain->end()
                               ? *std::prev(fx_mod_it)
                               : *next_fx_mod_it;
            }
        }();

        remove_fx_module(st, fx_chain_id, fx_mod_id);

        st.gui_state.focused_fx_mod_id = next_focused_fx_mod_id;
        if (!next_focused_fx_mod_id.valid())
        {
            st.gui_state.focused_fx_chain_id = {};
        }
    }
    else
    {
        remove_fx_module(st, fx_chain_id, fx_mod_id);
    }
}

} // namespace piejam::runtime::actions
