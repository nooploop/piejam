// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
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
    boxed_vector<runtime::mixer::channel_id> inputs;
    runtime::mixer::channel_id mainChannelId;
    runtime::mixer::channel_ids_t allChannelIds;
    std::unique_ptr<MixerChannel> mainChannel;
    MixerChannelsList inputChannels;
};

Mixer::Mixer(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              boxed_vector<runtime::mixer::channel_id>{},
              observe_once(runtime::selectors::select_mixer_main_channel),
              runtime::mixer::channel_ids_t{},
              std::make_unique<MixerChannel>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::select_mixer_main_channel))))
{
}

Mixer::~Mixer() = default;

auto
Mixer::inputChannels() -> MixerChannelsList*
{
    return &m_impl->inputChannels;
}

auto
Mixer::mainChannel() const -> MixerChannel*
{
    return m_impl->mainChannel.get();
}

void
Mixer::onSubscribe()
{
    observe(runtime::selectors::select_mixer_input_channels,
            [this](boxed_vector<runtime::mixer::channel_id> const& bus_infos) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->inputs, *bus_infos),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *inputChannels(),
                                [this](auto const& channel_id) {
                                    return std::make_unique<MixerChannel>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            channel_id);
                                }});

                m_impl->inputs = bus_infos;

                m_impl->allChannelIds.clear();

                // main channel must be added first, for correct order in
                // fxchains list
                m_impl->allChannelIds.emplace_back(m_impl->mainChannelId);
                boost::range::push_back(m_impl->allChannelIds, *m_impl->inputs);
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
            m_impl->allChannelIds));
}

} // namespace piejam::gui::model
