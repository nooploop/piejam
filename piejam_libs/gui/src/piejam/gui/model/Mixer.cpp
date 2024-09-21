// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Mixer.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/audio/types.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/request_mixer_levels_update.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <boost/range/algorithm_ext/push_back.hpp>

namespace piejam::gui::model
{

struct Mixer::Impl
{
    Impl(runtime::store_dispatch store_dispatch,
         runtime::subscriber& state_change_subscriber,
         runtime::mixer::channel_id main_channel)
        : main_channel_id{main_channel}
        , mainChannel{store_dispatch, state_change_subscriber, main_channel}
    {
    }

    runtime::mixer::channel_id main_channel_id;
    box<runtime::mixer::channel_ids_t> user_channel_ids;
    runtime::mixer::channel_ids_t all_channel_ids;

    MixerChannel mainChannel;
    MixerChannelsList userChannels;
};

Mixer::Mixer(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_impl{std::make_unique<Impl>(
              store_dispatch,
              state_change_subscriber,
              observe_once(runtime::selectors::select_mixer_main_channel))}
{
}

Mixer::~Mixer() = default;

auto
Mixer::userChannels() -> MixerChannelsList*
{
    return &m_impl->userChannels;
}

auto
Mixer::mainChannel() const -> MixerChannel*
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
                                *userChannels(),
                                [this](auto const& channel_id) {
                                    return std::make_unique<MixerChannel>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            channel_id);
                                }});

                m_impl->user_channel_ids = user_channel_ids;

                m_impl->all_channel_ids.clear();

                // main channel must be added first, for correct order in
                // fxchains list
                m_impl->all_channel_ids.emplace_back(m_impl->main_channel_id);
                boost::range::push_back(
                        m_impl->all_channel_ids,
                        *m_impl->user_channel_ids);
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

void
Mixer::requestLevelsUpdate()
{
    dispatch(runtime::actions::request_mixer_levels_update(
            m_impl->all_channel_ids));
}

} // namespace piejam::gui::model
