// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/AudioStreamListener.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QPointer>

namespace piejam::gui::model
{

class AudioStreamProvider : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioStreamListener* listener READ listener
                       WRITE setListener NOTIFY listenerChanged FINAL)

public:
    auto listener() const noexcept -> AudioStreamListener*
    {
        return m_listener;
    }

    void setListener(AudioStreamListener* x)
    {
        if (m_listener != x)
        {
            m_listener = x;
            emit listenerChanged();
        }
    }

    Q_INVOKABLE virtual void requestUpdate() = 0;

signals:

    void listenerChanged();

private:
    QPointer<AudioStreamListener> m_listener{};
};

} // namespace piejam::gui::model
