// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/Mixer.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/MixerChannel.h>
#include <piejam/audio/types.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/actions/request_mixer_levels_update.h>
#include <piejam/runtime/actions/set_bus_solo.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

struct Mixer::Impl
{
    boxed_vector<runtime::selectors::mixer_bus_info> inputs;
    boxed_vector<runtime::selectors::mixer_bus_info> outputs;
    runtime::mixer::bus_list_t all;
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

void
Mixer::onSubscribe()
{
    observe(runtime::selectors::make_bus_infos_selector(io_direction::input),
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
                                            io_direction::input,
                                            std::get<
                                                    runtime::device_io::bus_id>(
                                                    bus_info.in),
                                            bus_info.volume,
                                            bus_info.pan_balance,
                                            bus_info.mute,
                                            bus_info.level);
                                }});

                m_impl->inputs = bus_infos;
                m_impl->all.clear();
                append_bus_ids(*m_impl->inputs, m_impl->all);
                append_bus_ids(*m_impl->outputs, m_impl->all);
            });

    observe(runtime::selectors::make_bus_infos_selector(io_direction::output),
            [this](boxed_vector<runtime::selectors::mixer_bus_info> const&
                           bus_infos) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->outputs, *bus_infos),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *outputChannels(),
                                [this](auto const& bus_info) {
                                    return std::make_unique<MixerChannel>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_info.bus_id,
                                            io_direction::output,
                                            std::get<
                                                    runtime::device_io::bus_id>(
                                                    bus_info.out),
                                            bus_info.volume,
                                            bus_info.pan_balance,
                                            bus_info.mute,
                                            bus_info.level);
                                }});

                m_impl->outputs = bus_infos;
                m_impl->all.clear();
                append_bus_ids(*m_impl->inputs, m_impl->all);
                append_bus_ids(*m_impl->outputs, m_impl->all);
            });

    observe(runtime::selectors::select_input_solo_active,
            [this](bool const active) { setInputSoloActive(active); });

    observe(runtime::selectors::select_output_solo_active,
            [this](bool const active) { setOutputSoloActive(active); });
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
