// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MidiInputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/MidiDeviceConfig.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

MidiInputSettings::MidiInputSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
MidiInputSettings::onSubscribe()
{
    observe(runtime::selectors::select_midi_input_devices,
            [this](boxed_vector<midi::device_id_t> const& devs) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_devices, *devs),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *devices(),
                                [this](midi::device_id_t device_id) {
                                    return std::make_unique<MidiDeviceConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            device_id);
                                }});

                m_devices = devs;
            });
}

} // namespace piejam::app::gui::model
