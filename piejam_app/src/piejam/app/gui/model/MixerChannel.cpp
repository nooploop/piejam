// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/MixerChannel.h>

#include <piejam/app/gui/model/MidiAssignable.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/actions/select_fx_chain_bus.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/ui/thunk_action.h>

namespace piejam::app::gui::model
{

struct MixerChannel::Impl
{
    runtime::mixer::bus_id m_bus_id;
    runtime::float_parameter_id m_volume;
    runtime::float_parameter_id m_pan_balance;
    runtime::bool_parameter_id m_mute;
    runtime::stereo_level_parameter_id m_level;

    std::unique_ptr<MidiAssignable> m_volumeMidi;
    std::unique_ptr<MidiAssignable> m_panMidi;
    std::unique_ptr<MidiAssignable> m_muteMidi;

    boxed_vector<runtime::selectors::mixer_device_route> inputDevices;
    boxed_vector<runtime::selectors::mixer_channel_route> inputChannels;
    boxed_vector<runtime::selectors::mixer_device_route> outputDevices;
    boxed_vector<runtime::selectors::mixer_channel_route> outputChannels;
};

MixerChannel::MixerChannel(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::bus_id const id,
        runtime::float_parameter_id const volume,
        runtime::float_parameter_id const pan_balance,
        runtime::bool_parameter_id const mute,
        runtime::stereo_level_parameter_id const level)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              id,
              volume,
              pan_balance,
              mute,
              level,
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      volume),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      pan_balance),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      mute)))
{
}

MixerChannel::~MixerChannel() = default;

void
MixerChannel::onSubscribe()
{
    observe(runtime::selectors::make_mixer_bus_name_selector(m_impl->m_bus_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    //    observe(runtime::selectors::make_bus_type_selector(m_impl->m_device_bus_id),
    //            [this](audio::bus_type const bus_type) {
    //                setMono(bus_type == audio::bus_type::mono);
    //            });

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->m_volume),
            [this](float x) { setVolume(x); });

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->m_pan_balance),
            [this](float x) { setPanBalance(x); });

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->m_mute),
            [this](bool x) { setMute(x); });

    //    observe(runtime::selectors::make_solo_selector(m_io_dir, m_bus_id),
    //            [this](bool x) { setSolo(x); });

    observe(runtime::selectors::make_level_parameter_value_selector(
                    m_impl->m_level),
            [this](runtime::stereo_level const& x) {
                setLevel(x.left, x.right);
            });

    observe(runtime::selectors::make_mixer_channel_input_selector(
                    m_impl->m_bus_id),
            [this](boxed_string const& name) {
                setSelectedInput(QString::fromStdString(*name));
            });

    observe(runtime::selectors::select_mixer_input_devices,
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           inputDevices) {
                m_impl->inputDevices = inputDevices;

                QStringList inputDeviceNames;
                std::ranges::transform(
                        *inputDevices,
                        std::back_inserter(inputDeviceNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_device_route::name);
                setInputDevices(std::move(inputDeviceNames));
            });

    observe(runtime::selectors::make_mixer_input_channels_selector(
                    m_impl->m_bus_id),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           inputChannels) {
                m_impl->inputChannels = inputChannels;

                QStringList inputChannelNames;
                std::ranges::transform(
                        *inputChannels,
                        std::back_inserter(inputChannelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setInputChannels(std::move(inputChannelNames));
            });

    observe(runtime::selectors::make_mixer_channel_output_selector(
                    m_impl->m_bus_id),
            [this](boxed_string const& name) {
                setSelectedOutput(QString::fromStdString(*name));
            });

    observe(runtime::selectors::select_mixer_output_devices,
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           outputDevices) {
                m_impl->outputDevices = outputDevices;

                QStringList outputDeviceNames;
                std::ranges::transform(
                        *outputDevices,
                        std::back_inserter(outputDeviceNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_device_route::name);
                setOutputDevices(std::move(outputDeviceNames));
            });

    observe(runtime::selectors::make_mixer_output_channels_selector(
                    m_impl->m_bus_id),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           outputChannels) {
                m_impl->outputChannels = outputChannels;

                QStringList outputChannelNames;
                std::ranges::transform(
                        *outputChannels,
                        std::back_inserter(outputChannelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setOutputChannels(std::move(outputChannelNames));
            });
}

void
MixerChannel::changeName(QString const& name)
{
    runtime::actions::set_mixer_bus_name action;
    action.bus_id = m_impl->m_bus_id;
    action.name = name.toStdString();
    dispatch(action);
}

void
MixerChannel::changeVolume(double value)
{
    dispatch(runtime::actions::set_float_parameter(
            m_impl->m_volume,
            static_cast<float>(value)));
}

void
MixerChannel::changePanBalance(double value)
{
    dispatch(runtime::actions::set_float_parameter(
            m_impl->m_pan_balance,
            static_cast<float>(value)));
}

void
MixerChannel::changeMute(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->m_mute, value));
}

void
MixerChannel::focusFxChain()
{
    runtime::actions::select_fx_chain_bus action;
    action.bus_id = m_impl->m_bus_id;
    dispatch(action);
}

void
MixerChannel::deleteChannel()
{
    runtime::actions::delete_mixer_bus action;
    action.bus_id = m_impl->m_bus_id;
    dispatch(action);
}

auto
MixerChannel::volumeMidi() const -> piejam::gui::model::MidiAssignable*
{
    return m_impl->m_volumeMidi.get();
}

auto
MixerChannel::panMidi() const -> piejam::gui::model::MidiAssignable*
{
    return m_impl->m_panMidi.get();
}

auto
MixerChannel::muteMidi() const -> piejam::gui::model::MidiAssignable*
{
    return m_impl->m_muteMidi.get();
}

void
MixerChannel::changeInputToMix()
{
    runtime::actions::set_mixer_channel_input action;
    action.bus_id = m_impl->m_bus_id;
    dispatch(action);
}

void
MixerChannel::changeInputToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->inputDevices->size());

    runtime::actions::set_mixer_channel_input action;
    action.bus_id = m_impl->m_bus_id;
    action.route = (*m_impl->inputDevices)[index].bus_id;
    dispatch(action);
}

void
MixerChannel::changeInputToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->inputChannels->size());

    runtime::actions::set_mixer_channel_input action;
    action.bus_id = m_impl->m_bus_id;
    action.route = (*m_impl->inputChannels)[index].bus_id;
    dispatch(action);
}

void
MixerChannel::changeOutputToNone()
{
    runtime::actions::set_mixer_channel_output action;
    action.bus_id = m_impl->m_bus_id;
    dispatch(action);
}

void
MixerChannel::changeOutputToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->outputDevices->size());

    runtime::actions::set_mixer_channel_output action;
    action.bus_id = m_impl->m_bus_id;
    action.route = (*m_impl->outputDevices)[index].bus_id;
    dispatch(action);
}

void
MixerChannel::changeOutputToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->outputChannels->size());

    runtime::actions::set_mixer_channel_output action;
    action.bus_id = m_impl->m_bus_id;
    action.route = (*m_impl->outputChannels)[index].bus_id;
    dispatch(action);
}

} // namespace piejam::app::gui::model
