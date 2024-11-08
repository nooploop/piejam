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

class AudioRoutingSelection final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

public:
    enum class State
    {
        Invalid,
        Valid,
        NotMixed
    };

    Q_ENUM(State)

private:
    M_PIEJAM_GUI_PROPERTY(bool, isDefault, setDefault)
    M_PIEJAM_GUI_PROPERTY(State, state, setState)
    M_PIEJAM_GUI_PROPERTY(QString, label, setLabel)

public:
    AudioRoutingSelection(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id,
            runtime::mixer::io_socket);

private:
    void onSubscribe() override;

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::model
