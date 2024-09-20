// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MidiInputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/MidiDeviceConfig.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MidiInputSettings::Impl
{
    box<midi::device_ids_t> device_ids;
    MidiDeviceList devices;
};

MidiInputSettings::MidiInputSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>())
{
}

MidiInputSettings::~MidiInputSettings() = default;

auto
MidiInputSettings::devices() -> MidiDeviceList*
{
    return &m_impl->devices;
}

void
MidiInputSettings::onSubscribe()
{
    observe(runtime::selectors::select_midi_input_devices,
            [this](auto const& devs) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->device_ids, *devs),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *devices(),
                                [this](midi::device_id_t device_id) {
                                    return std::make_unique<MidiDeviceConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            device_id);
                                }});

                m_impl->device_ids = devs;
            });
}

} // namespace piejam::gui::model
