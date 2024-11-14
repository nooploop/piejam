// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelAuxSend.h>

#include <piejam/gui/generic_list_model_edit_script_executor.h>
#include <piejam/gui/model/AudioRouting.h>
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
        : aux{store_dispatch,
              state_change_subscriber,
              id,
              runtime::mixer::io_socket::aux}
    {
    }

    AudioRouting aux;

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
}

auto
MixerChannelAuxSend::aux() const noexcept -> AudioRouting*
{
    return &m_impl->aux;
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

} // namespace piejam::gui::model
