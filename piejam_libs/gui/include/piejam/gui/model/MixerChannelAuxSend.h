// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/MixerChannel.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

class QAbstractListModel;

namespace piejam::gui::model
{

class MixerChannelAuxSend final : public MixerChannel
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::AudioRoutingSelection*,
            selected)
    M_PIEJAM_GUI_PROPERTY(bool, canToggle, setCanToggle)
    M_PIEJAM_GUI_PROPERTY(bool, enabled, setEnabled)
    Q_PROPERTY(piejam::gui::model::FloatParameter* volume READ volume NOTIFY
                       volumeChanged FINAL)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(QAbstractListModel*, devices)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(QAbstractListModel*, channels)

public:
    MixerChannelAuxSend(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);

    auto volume() const noexcept -> FloatParameter*;

    Q_INVOKABLE void toggleEnabled();
    Q_INVOKABLE void changeToDevice(unsigned index);
    Q_INVOKABLE void changeToChannel(unsigned index);

signals:
    void volumeChanged();

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
