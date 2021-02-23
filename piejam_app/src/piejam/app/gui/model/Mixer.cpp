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
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

struct Mixer::Impl
{
    boxed_vector<runtime::selectors::mixer_bus_info> inputs;
    box<runtime::selectors::mixer_bus_info> mainBusInfo;
    runtime::mixer::bus_list_t all;
    std::unique_ptr<piejam::app::gui::model::MixerChannel> mainChannel;
};

static auto
append_bus_ids(
        std::vector<runtime::selectors::mixer_bus_info> const& bus_infos,
        runtime::mixer::bus_list_t& bus_ids)
{
    std::ranges::transform(
            bus_infos,
            std::back_inserter(bus_ids),
            &runtime::selectors::mixer_bus_info::bus_id);
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
    observe(runtime::selectors::select_mixer_bus_infos,
            [this](boxed_vector<runtime::selectors::mixer_bus_info> const&
                           bus_infos) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->inputs, *bus_infos),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *inputChannels(),
                                [this](auto const& bus_info) {
                                    return std::make_unique<MixerChannel>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_info.bus_id,
                                            bus_info.volume,
                                            bus_info.pan_balance,
                                            bus_info.mute,
                                            bus_info.level);
                                }});

                m_impl->inputs = bus_infos;

                m_impl->all.clear();
                append_bus_ids(*m_impl->inputs, m_impl->all);
                m_impl->all.emplace_back(m_impl->mainBusInfo->bus_id);
            });

    observe(runtime::selectors::select_mixer_main_bus_info,
            [this](box<runtime::selectors::mixer_bus_info> const& mainBusInfo) {
                if (mainBusInfo != m_impl->mainBusInfo)
                {
                    m_impl->mainChannel = std::make_unique<MixerChannel>(
                            dispatch(),
                            state_change_subscriber(),
                            mainBusInfo->bus_id,
                            mainBusInfo->volume,
                            mainBusInfo->pan_balance,
                            mainBusInfo->mute,
                            mainBusInfo->level);

                    emit mainChannelChanged();

                    m_impl->mainBusInfo = mainBusInfo;

                    m_impl->all.clear();
                    append_bus_ids(*m_impl->inputs, m_impl->all);
                    m_impl->all.emplace_back(m_impl->mainBusInfo->bus_id);
                }
            });

    observe(runtime::selectors::select_input_solo_active,
            [this](bool const active) { setInputSoloActive(active); });

    observe(runtime::selectors::select_output_solo_active,
            [this](bool const active) { setOutputSoloActive(active); });
}

void
Mixer::addChannel(QString const& newChannelName)
{
    runtime::actions::add_mixer_bus action;
    action.name = newChannelName.toStdString();
    dispatch(action);
}

void
Mixer::setInputSolo(unsigned const index)
{
    runtime::actions::set_input_bus_solo action;
    action.index = index;
    dispatch(action);
}

void
Mixer::setOutputSolo(unsigned const index)
{
    runtime::actions::set_output_bus_solo action;
    action.index = index;
    dispatch(action);
}

void
Mixer::requestLevelsUpdate()
{
    dispatch(runtime::actions::request_mixer_levels_update(m_impl->all));
}

} // namespace piejam::app::gui::model
