// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioInputOutputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/audio/types.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/actions/add_bus.h>
#include <piejam/runtime/selectors.h>

#include <string>

namespace piejam::gui::model
{

struct AudioInputOutputSettings::Impl
{
    io_direction ioDir;
    runtime::device_io::bus_list_t busIds;
    BusConfigsList busConfigs;
};

AudioInputOutputSettings::AudioInputOutputSettings(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        io_direction const settings_type)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(settings_type))
{
}

AudioInputOutputSettings::~AudioInputOutputSettings() = default;

auto
AudioInputOutputSettings::busConfigs() -> BusConfigsList*
{
    return &m_impl->busConfigs;
}

void
AudioInputOutputSettings::onSubscribe()
{
    namespace selectors = runtime::selectors;

    observe(selectors::make_num_device_channels_selector(m_impl->ioDir),
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                {
                    channels.push_back(QString::number(n + 1));
                }

                setChannels(channels);
            });

    observe(selectors::make_device_bus_list_selector(m_impl->ioDir),
            [this](box<runtime::device_io::bus_list_t> const& bus_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(m_impl->busIds, *bus_ids),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->busConfigs,
                                [this](runtime::device_io::bus_id bus_id) {
                                    return std::make_unique<BusConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            bus_id);
                                }});

                m_impl->busIds = bus_ids;
            });
}

static void
addBus(runtime::store_dispatch dispatch,
       io_direction direction,
       audio::bus_type bus_type)
{
    runtime::actions::add_bus action;
    action.direction = direction;
    action.type = bus_type;
    dispatch(action);
}

void
AudioInputOutputSettings::addMonoBus()
{
    addBus(dispatch(), m_impl->ioDir, audio::bus_type::mono);
}

void
AudioInputOutputSettings::addStereoBus()
{
    addBus(dispatch(), m_impl->ioDir, audio::bus_type::stereo);
}

} // namespace piejam::gui::model
