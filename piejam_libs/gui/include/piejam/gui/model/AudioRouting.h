// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

namespace piejam::gui::model
{

class AudioRouting final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(QString, defaultName)
    M_PIEJAM_GUI_PROPERTY(bool, defaultIsValid, setDefaultIsValid)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            piejam::gui::model::AudioRoutingSelection*,
            selected)
    M_PIEJAM_GUI_PROPERTY(QStringList, devices, setDevices)
    M_PIEJAM_GUI_PROPERTY(QStringList, channels, setChannels)

public:
    AudioRouting(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id,
            runtime::mixer::io_socket);

    Q_INVOKABLE void changeToDefault();
    Q_INVOKABLE void changeToDevice(unsigned index);
    Q_INVOKABLE void changeToChannel(unsigned index);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;

    QString m_defaultName;
};

} // namespace piejam::gui::model
