// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/mixer_actions.h>

#include <piejam/runtime/state.h>

#include <iterator>

namespace piejam::runtime::actions
{

auto
add_mixer_channel::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::add_mixer_channel(new_st, name);

    return new_st;
}

auto
delete_mixer_channel::reduce(state const& st) const -> state
{
    auto new_st = st;

    runtime::remove_mixer_channel(new_st, channel_id);

    return new_st;
}

auto
set_mixer_channel_name::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.channels.update(channel_id, [this](mixer::channel& bus) {
        bus.name = name;
    });

    return new_st;
}

template <io_direction D>
auto
set_mixer_channel_route<D>::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.channels.update(channel_id, [this](mixer::channel& bus) {
        (D == io_direction::input ? bus.in : bus.out) = route;
    });

    return new_st;
}

template auto
set_mixer_channel_route<io_direction::input>::reduce(state const&) const
        -> state;
template auto
set_mixer_channel_route<io_direction::output>::reduce(state const&) const
        -> state;

auto
move_mixer_channel_left::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.inputs.update([this](mixer::channel_ids_t& channel_ids) {
        auto it = std::ranges::find(channel_ids, channel_id);
        BOOST_ASSERT(it != channel_ids.end());
        BOOST_ASSERT(it != channel_ids.begin());
        std::iter_swap(it, std::prev(it));
    });

    return new_st;
}

auto
move_mixer_channel_right::reduce(state const& st) const -> state
{
    auto new_st = st;

    new_st.mixer_state.inputs.update([this](mixer::channel_ids_t& channel_ids) {
        auto it = std::ranges::find(channel_ids, channel_id);
        BOOST_ASSERT(it != channel_ids.end());
        BOOST_ASSERT(std::next(it) != channel_ids.begin());
        std::iter_swap(it, std::next(it));
    });

    return new_st;
}

} // namespace piejam::runtime::actions
