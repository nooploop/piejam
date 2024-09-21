// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class AudioRoutingSelection final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(State state READ state NOTIFY stateChanged FINAL)
    Q_PROPERTY(QString label READ label NOTIFY labelChanged FINAL)

public:
    AudioRoutingSelection(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id,
            runtime::mixer::io_socket);
    ~AudioRoutingSelection() override;

    enum class State
    {
        Invalid,
        Valid,
        NotMixed
    };

    Q_ENUM(State)

    auto state() const noexcept -> State
    {
        return m_state;
    }

    void setState(State x)
    {
        if (m_state != x)
        {
            m_state = x;
            emit stateChanged();
        }
    }

    auto label() const noexcept -> QString const&
    {
        return m_label;
    }

    void setLabel(QString const& x)
    {
        if (m_label != x)
        {
            m_label = x;
            emit labelChanged();
        }
    }

signals:
    void stateChanged();
    void labelChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    State m_state{};
    QString m_label;
};

} // namespace piejam::gui::model
