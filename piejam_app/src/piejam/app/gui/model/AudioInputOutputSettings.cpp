// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/AudioInputOutputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/app/gui/model/BusConfig.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/selectors.h>

#include <string>

namespace piejam::app::gui::model
{

AudioInputOutputSettings::AudioInputOutputSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        io_direction const settings_type)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_settings_type(settings_type)
{
}

void
AudioInputOutputSettings::onSubscribe()
{
    namespace selectors = runtime::selectors;

    observe(selectors::make_num_device_channels_selector(m_settings_type),
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                    channels.push_back(QString::number(n + 1));

                setChannels(channels);
            });

    observe(selectors::make_device_bus_list_selector(m_settings_type),
            [this](box<runtime::device_io::bus_list_t> const& bus_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(m_bus_ids, *bus_ids),
                        piejam::gui::generic_list_model_edit_script_executor{
                                *busConfigs(),
                                [this](runtime::device_io::bus_id bus_id) {
                                    return std::make_unique<BusConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_id);
                                }});

                m_bus_ids = bus_ids;
            });
}

void
AudioInputOutputSettings::addBus(audio::bus_type bus_type)
{
    runtime::actions::add_bus action;
    action.direction = m_settings_type;
    action.type = bus_type;
    dispatch(action);
}

void
AudioInputOutputSettings::addMonoBus()
{
    addBus(audio::bus_type::mono);
}

void
AudioInputOutputSettings::addStereoBus()
{
    addBus(audio::bus_type::stereo);
}

} // namespace piejam::app::gui::model
