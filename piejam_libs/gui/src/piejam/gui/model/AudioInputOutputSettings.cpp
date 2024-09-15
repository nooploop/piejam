// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/AudioInputOutputSettings.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/audio/types.h>
#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/ExternalAudioDeviceConfig.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/runtime/actions/external_audio_device_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct AudioInputOutputSettings::Impl
{
    io_direction io_dir;
    unique_box<runtime::external_audio::device_ids_t> device_ids;

    ExternalAudioDeviceConfigList deviceConfigs;
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
AudioInputOutputSettings::deviceConfigs() -> ExternalAudioDeviceConfigList*
{
    return &m_impl->deviceConfigs;
}

void
AudioInputOutputSettings::onSubscribe()
{
    namespace selectors = runtime::selectors;

    observe(selectors::make_num_device_channels_selector(m_impl->io_dir),
            [this](std::size_t const num_input_channels) {
                QStringList channels;
                channels.push_back("-");
                for (std::size_t n = 0; n < num_input_channels; ++n)
                {
                    channels.push_back(QString::number(n + 1));
                }

                setChannels(channels);
            });

    observe(selectors::make_external_audio_device_ids_selector(m_impl->io_dir),
            [this](unique_box<runtime::external_audio::device_ids_t> const&
                           device_ids) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(
                                *m_impl->device_ids,
                                *device_ids),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->deviceConfigs,
                                [this](runtime::external_audio::device_id
                                               device_id) {
                                    return std::make_unique<
                                            ExternalAudioDeviceConfig>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            device_id);
                                }});

                m_impl->device_ids = device_ids;
            });
}

static void
addDevice(
        runtime::store_dispatch dispatch,
        io_direction direction,
        audio::bus_type bus_type)
{
    runtime::actions::add_external_audio_device action;
    action.direction = direction;
    action.type = bus_type;
    dispatch(action);
}

void
AudioInputOutputSettings::addMonoDevice()
{
    addDevice(dispatch(), m_impl->io_dir, audio::bus_type::mono);
}

void
AudioInputOutputSettings::addStereoDevice()
{
    addDevice(dispatch(), m_impl->io_dir, audio::bus_type::stereo);
}

} // namespace piejam::gui::model
