// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Mixer.h>

#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/MixerChannelModels.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/audio/types.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct Mixer::Impl
{
    Impl(runtime::store_dispatch store_dispatch,
         runtime::subscriber& state_change_subscriber,
         runtime::mixer::channel_id main_channel)
        : mainChannel{store_dispatch, state_change_subscriber, main_channel}
    {
    }

    box<runtime::mixer::channel_ids_t> user_channel_ids;

    MixerChannelModels mainChannel;
    MixerChannelsList userChannels;
};

Mixer::Mixer(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_impl{make_pimpl<Impl>(
              store_dispatch,
              state_change_subscriber,
              observe_once(runtime::selectors::select_mixer_main_channel))}
{
}

auto
Mixer::userChannels() const noexcept -> QAbstractListModel*
{
    return &m_impl->userChannels;
}

auto
Mixer::mainChannel() const noexcept -> MixerChannelModels*
{
    return &m_impl->mainChannel;
}

void
Mixer::onSubscribe()
{
    observe(runtime::selectors::select_mixer_user_channels,
            [this](box<runtime::mixer::channel_ids_t> const& user_channel_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                *m_impl->user_channel_ids,
                                *user_channel_ids),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->userChannels,
                                [this](auto const& channel_id) {
                                    return std::make_unique<MixerChannelModels>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            channel_id);
                                }});

                m_impl->user_channel_ids = user_channel_ids;
            });
}

void
Mixer::addMonoChannel(QString const& newChannelName)
{
    runtime::actions::add_mixer_channel action;
    action.name = newChannelName.toStdString();
    action.bus_type = audio::bus_type::mono;
    action.auto_assign_input = true;
    dispatch(action);
}

void
Mixer::addStereoChannel(QString const& newChannelName)
{
    runtime::actions::add_mixer_channel action;
    action.name = newChannelName.toStdString();
    action.bus_type = audio::bus_type::stereo;
    action.auto_assign_input = true;
    dispatch(action);
}

} // namespace piejam::gui::model
