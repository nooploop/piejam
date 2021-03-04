// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/Mixer.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/MixerChannel.h>
#include <piejam/audio/types.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/request_mixer_levels_update.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

struct Mixer::Impl
{
    boxed_vector<runtime::selectors::mixer_channel_info> inputs;
    box<runtime::selectors::mixer_channel_info> mainChannelInfo;
    runtime::mixer::channel_ids_t allChannelIds;
    std::unique_ptr<piejam::app::gui::model::MixerChannel> mainChannel;
};

static auto
append_channel_ids(
        std::vector<runtime::selectors::mixer_channel_info> const&
                channel_infos,
        runtime::mixer::channel_ids_t& channel_ids)
{
    std::ranges::transform(
            channel_infos,
            std::back_inserter(channel_ids),
            &runtime::selectors::mixer_channel_info::channel_id);
}

Mixer::Mixer(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>())
{
}

Mixer::~Mixer() = default;

auto
Mixer::mainChannel() const -> piejam::gui::model::MixerChannel*
{
    return m_impl->mainChannel.get();
}

void
Mixer::onSubscribe()
{
    observe(runtime::selectors::select_mixer_channel_infos,
            [this](boxed_vector<runtime::selectors::mixer_channel_info> const&
                           bus_infos) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->inputs, *bus_infos),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *inputChannels(),
                                [this](auto const& bus_info) {
                                    return std::make_unique<MixerChannel>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_info.channel_id,
                                            bus_info.volume,
                                            bus_info.pan_balance,
                                            bus_info.mute,
                                            bus_info.solo,
                                            bus_info.level);
                                }});

                m_impl->inputs = bus_infos;

                m_impl->allChannelIds.clear();
                append_channel_ids(*m_impl->inputs, m_impl->allChannelIds);
                m_impl->allChannelIds.emplace_back(
                        m_impl->mainChannelInfo->channel_id);
            });

    observe(runtime::selectors::select_mixer_main_channel_info,
            [this](box<runtime::selectors::mixer_channel_info> const&
                           mainBusInfo) {
                if (mainBusInfo != m_impl->mainChannelInfo)
                {
                    m_impl->mainChannel = std::make_unique<MixerChannel>(
                            dispatch(),
                            state_change_subscriber(),
                            mainBusInfo->channel_id,
                            mainBusInfo->volume,
                            mainBusInfo->pan_balance,
                            mainBusInfo->mute,
                            mainBusInfo->solo,
                            mainBusInfo->level);

                    emit mainChannelChanged();

                    m_impl->mainChannelInfo = mainBusInfo;

                    m_impl->allChannelIds.clear();
                    append_channel_ids(*m_impl->inputs, m_impl->allChannelIds);
                    m_impl->allChannelIds.emplace_back(
                            m_impl->mainChannelInfo->channel_id);
                }
            });
}

void
Mixer::addChannel(QString const& newChannelName)
{
    runtime::actions::add_mixer_channel action;
    action.name = newChannelName.toStdString();
    dispatch(action);
}

void
Mixer::requestLevelsUpdate()
{
    dispatch(runtime::actions::request_mixer_levels_update(
            m_impl->allChannelIds));
}

} // namespace piejam::app::gui::model
