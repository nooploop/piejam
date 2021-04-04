// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class AudioStreamProvider : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(AudioStreamListener* listener READ listener WRITE setListener
                       NOTIFY listenerChanged FINAL)

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

signals:

    void listenerChanged();

private:
    AudioStreamListener* m_listener{};
};

} // namespace piejam::gui::model
