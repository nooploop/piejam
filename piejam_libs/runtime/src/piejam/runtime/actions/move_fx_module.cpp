// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/move_fx_module.h>

#include <piejam/runtime/state.h>

#include <algorithm>

namespace piejam::runtime::actions
{

auto
move_fx_module_left::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.channels.update(
            st.fx_chain_channel,
            [this](mixer::channel& bus) {
                bus.fx_chain.update([this](fx::chain_t& fx_chain) {
                    if (auto it = std::ranges::find(fx_chain, fx_mod_id);
                        it != fx_chain.end() && it != fx_chain.begin())
                    {
                        std::iter_swap(it, std::prev(it));
                    }
                });
            });

    return new_st;
}

auto
move_fx_module_right::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.channels.update(
            st.fx_chain_channel,
            [this](mixer::channel& bus) {
                bus.fx_chain.update([this](fx::chain_t& fx_chain) {
                    if (auto it = std::ranges::find(fx_chain, fx_mod_id);
                        it != fx_chain.end() && std::next(it) != fx_chain.end())
                    {
                        std::iter_swap(it, std::next(it));
                    }
                });
            });

    return new_st;
}

} // namespace piejam::runtime::actions
