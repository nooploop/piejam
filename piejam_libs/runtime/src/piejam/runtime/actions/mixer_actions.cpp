// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/actions/mixer_actions.h>

#include <piejam/runtime/actions/delete_fx_module.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/hof/unpack.hpp>

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
            auto it = std::ranges::find_if(
                    st.mixer_state.channels,
                    boost::hof::unpack(
                            [device_id](
                                    mixer::channel_id,
                                    mixer::channel const& mixer_channel) {
                                return std::holds_alternative<
                                               external_audio::device_id>(
                                               mixer_channel.in) &&
                                       std::get<external_audio::device_id>(
                                               mixer_channel.in) == device_id;
                            }));

            if (it == st.mixer_state.channels.end() &&
                st.external_audio_state.devices[device_id].bus_type == bus_type)
            {
                st.mixer_state.channels.update(
                        added_mixer_channel_id,
                        [device_id](mixer::channel& mixer_channel) {
                            mixer_channel.in = device_id;
                        });
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

auto
initiate_mixer_channel_deletion(mixer::channel_id mixer_channel_id)
        -> thunk_action
{
    return [=](auto&& get_state, auto&& dispatch) {
        state const& st = get_state();
        mixer::channel const& mixer_channel =
                st.mixer_state.channels[mixer_channel_id];
        if (mixer_channel.fx_chain->empty())
        {
            delete_mixer_channel action;
            action.mixer_channel_id = mixer_channel_id;
            dispatch(action);
        }
        else
        {
            batch_action batch;
            for (fx::module_id const fx_mod_id : *mixer_channel.fx_chain)
            {
                auto delete_fx_mod = std::make_unique<delete_fx_module>();
                delete_fx_mod->fx_chain_id = mixer_channel_id;
                delete_fx_mod->fx_mod_id = fx_mod_id;
                batch.push_back(std::move(delete_fx_mod));
            }

            auto action = std::make_unique<delete_mixer_channel>();
            action->mixer_channel_id = mixer_channel_id;
            batch.push_back(std::move(action));

            dispatch(batch);
        }
    };
}

void
set_mixer_channel_name::reduce(state& st) const
{
    st.mixer_state.channels.update(
            channel_id,
            [this](mixer::channel& mixer_channel) {
                mixer_channel.name = name;
            });
}

void
set_mixer_channel_route::reduce(state& st) const
{
    st.mixer_state.channels.update(
            channel_id,
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
                        BOOST_ASSERT_MSG(
                                false,
                                "use select_mixer_channel_aux_route");
                        break;
                }
            });
}

void
select_mixer_channel_aux_route::reduce(state& st) const
{
    st.mixer_state.channels.update(
            channel_id,
            [this](mixer::channel& mixer_channel) {
                mixer_channel.aux = route;
            });
}

void
enable_mixer_channel_aux_route::reduce(state& st) const
{
    st.mixer_state.channels.update(
            channel_id,
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
            });
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
