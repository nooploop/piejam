// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelAuxSend.h>

#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/AudioRoutingSelection.h>
#include <piejam/gui/model/FloatParameter.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/String.h>

#include <piejam/algorithm/edit_script.h>
#include <piejam/runtime/actions/mixer_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MixerChannelAuxSend::Impl
{
    Impl(runtime::store_dispatch store_dispatch,
         runtime::subscriber& state_change_subscriber,
         runtime::mixer::channel_id const id)
        : selected{
                  store_dispatch,
                  state_change_subscriber,
                  id,
                  runtime::mixer::io_socket::aux}
    {
    }

    AudioRoutingSelection selected;
    Strings devicesList{};

    boxed_vector<runtime::selectors::mixer_device_route> devices;
    boxed_vector<runtime::selectors::mixer_channel_route> channels;
    runtime::float_parameter_id volume_id;

    std::unique_ptr<FloatParameter> volume;
};

MixerChannelAuxSend::MixerChannelAuxSend(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : MixerChannel{store_dispatch, state_change_subscriber, id}
    , m_impl{make_pimpl<Impl>(store_dispatch, state_change_subscriber, id)}
{
    connectSubscribableChild(m_impl->selected);
}

auto
MixerChannelAuxSend::selected() const noexcept -> AudioRoutingSelection*
{
    return &m_impl->selected;
}

auto
MixerChannelAuxSend::devices() const noexcept -> QAbstractListModel*
{
    return &m_impl->devicesList;
}

auto
MixerChannelAuxSend::volume() const noexcept -> FloatParameter*
{
    return m_impl->volume.get();
}

void
MixerChannelAuxSend::onSubscribe()
{
    MixerChannel::onSubscribe();

    observe(runtime::selectors::make_mixer_device_routes_selector(
                    audio::bus_type::stereo,
                    runtime::mixer::io_socket::aux),
            [this](boxed_vector<runtime::selectors::mixer_device_route> const&
                           devices) {
                algorithm::apply_edit_script(
                        algorithm::edit_script(*m_impl->devices, *devices),
                        piejam::gui::generic_list_model_edit_script_executor{
                                m_impl->devicesList,
                                [this](auto const& route) {
                                    return std::make_unique<String>(
                                            dispatch(),
                                            state_change_subscriber(),
                                            route.name);
                                }});

                m_impl->devices = devices;
            });

    observe(runtime::selectors::make_mixer_channel_routes_selector(
                    channel_id(),
                    runtime::mixer::io_socket::aux),
            [this](boxed_vector<runtime::selectors::mixer_channel_route> const&
                           channels) {
                m_impl->channels = channels;

                QStringList channelNames;
                std::ranges::transform(
                        *channels,
                        std::back_inserter(channelNames),
                        &QString::fromStdString,
                        &runtime::selectors::mixer_channel_route::name);
                setChannels(std::move(channelNames));
            });

    observe(runtime::selectors::make_mixer_channel_aux_enabled_selector(
                    channel_id()),
            [this](bool const enabled) { setEnabled(enabled); });

    observe(runtime::selectors::make_mixer_channel_can_toggle_aux_selector(
                    channel_id()),
            [this](bool const x) { setCanToggle(x); });

    observe(runtime::selectors::
                    make_mixer_channel_aux_volume_parameter_selector(
                            channel_id()),
            [this](runtime::float_parameter_id const volume_id) {
                if (m_impl->volume_id != volume_id)
                {
                    auto volume = volume_id.valid()
                                          ? std::make_unique<FloatParameter>(
                                                    dispatch(),
                                                    state_change_subscriber(),
                                                    volume_id)
                                          : nullptr;
                    m_impl->volume_id = volume_id;
                    std::swap(m_impl->volume, volume);
                    emit volumeChanged();
                }
            });
}

void
MixerChannelAuxSend::toggleEnabled()
{
    runtime::actions::enable_mixer_channel_aux_route action;
    action.channel_id = channel_id();
    action.enabled = !m_enabled;
    dispatch(action);
}

void
MixerChannelAuxSend::changeToDevice(unsigned index)
{
    BOOST_ASSERT(index < m_impl->devices->size());

    runtime::actions::select_mixer_channel_aux_route action;
    action.channel_id = channel_id();
    action.route = (*m_impl->devices)[index].device_id;
    dispatch(action);
}

void
MixerChannelAuxSend::changeToChannel(unsigned index)
{
    BOOST_ASSERT(index < m_impl->channels->size());

    runtime::actions::select_mixer_channel_aux_route action;
    action.channel_id = channel_id();
    action.route = (*m_impl->channels)[index].channel_id;
    dispatch(action);
}

} // namespace piejam::gui::model
