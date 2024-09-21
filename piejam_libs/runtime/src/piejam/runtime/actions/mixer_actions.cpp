// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/mixer_actions.h>

#include <piejam/functional/get.h>
#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/state.h>

#include <boost/hof/compose.hpp>

#include <iterator>

namespace piejam::runtime::actions
{

void
add_mixer_channel::reduce(state& st) const
{
    auto added_mixer_channel_id =
            runtime::add_mixer_channel(st, name, bus_type);

    if (auto_assign_input)
    {
        for (auto device_id : *st.external_audio_state.inputs)
        {
            auto it = std::ranges::find(
                    st.mixer_state.channels,
                    mixer::io_address_t{device_id},
                    boost::hof::compose(&mixer::channel::in, get_by_index<1>));

            if (it == st.mixer_state.channels.end() &&
                st.external_audio_state.devices[device_id].bus_type == bus_type)
            {
                st.mixer_state.channels.lock()[added_mixer_channel_id].in =
                        device_id;
                break;
            }
        }
    }
}

void
delete_mixer_channel::reduce(state& st) const
{
    runtime::remove_mixer_channel(st, mixer_channel_id);
}

void
set_mixer_channel_name::reduce(state& st) const
{
    st.mixer_state.channels.lock()[channel_id].name = name;
}

void
set_mixer_channel_route::reduce(state& st) const
{
    [this](mixer::channel& mixer_channel) {
        switch (io_socket)
        {
            case mixer::io_socket::in:
                mixer_channel.in = route;
                break;

            case mixer::io_socket::out:
                mixer_channel.out = route;
                break;

            case mixer::io_socket::aux:
                BOOST_ASSERT_MSG(false, "use select_mixer_channel_aux_route");
                break;
        }
    }(st.mixer_state.channels.lock()[channel_id]);
}

void
select_mixer_channel_aux_route::reduce(state& st) const
{
    st.mixer_state.channels.lock()[channel_id].aux = route;
}

void
enable_mixer_channel_aux_route::reduce(state& st) const
{
    [this](mixer::channel& mixer_channel) {
        auto aux_sends = mixer_channel.aux_sends.lock();
        if (auto it = aux_sends->find(mixer_channel.aux);
            it != aux_sends->end())
        {
            it->second.enabled = enabled;
        }
        else
        {
            BOOST_ASSERT(false);
        }
    }(st.mixer_state.channels.lock()[channel_id]);
}

void
move_mixer_channel_left::reduce(state& st) const
{
    auto channel_ids = st.mixer_state.inputs.lock();

    auto it = std::ranges::find(*channel_ids, channel_id);
    BOOST_ASSERT(it != channel_ids->end());
    BOOST_ASSERT(it != channel_ids->begin());
    std::iter_swap(it, std::prev(it));
}

void
move_mixer_channel_right::reduce(state& st) const
{
    auto channel_ids = st.mixer_state.inputs.lock();

    auto it = std::ranges::find(*channel_ids, channel_id);
    BOOST_ASSERT(it != channel_ids->end());
    BOOST_ASSERT(std::next(it) != channel_ids->begin());
    std::iter_swap(it, std::next(it));
}

} // namespace piejam::runtime::actions
