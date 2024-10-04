// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannel.h>

#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

MixerChannel::MixerChannel(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_channel_id{id}
    , m_busType{toBusType(observe_once(
              runtime::selectors::make_mixer_channel_bus_type_selector(id)))}
    , m_color{static_cast<MaterialColor>(observe_once(
              runtime::selectors::make_mixer_channel_color_selector(id)))}
{
}

MixerChannel::~MixerChannel() = default;

void
MixerChannel::onSubscribe()
{
    observe(runtime::selectors::make_mixer_channel_name_selector(m_channel_id),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_mixer_channel_color_selector(m_channel_id),
            [this](runtime::material_color color) {
                setColor(static_cast<MaterialColor>(color));
            });
}

} // namespace piejam::gui::model
