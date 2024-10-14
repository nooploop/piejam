// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Waveform.h>

#include <QObject>

namespace piejam::gui::model
{

class WaveformSlot final : public QObject
{
    Q_OBJECT

public:
    [[nodiscard]]
    auto get() const noexcept -> Waveform const&
    {
        return m_waveform;
    }

    void update(Waveform const& w)
    {
        m_waveform.shift_push_back(w);
        emit changed();
    }

    void resize(std::size_t size)
    {
        m_waveform.resize(size);
        emit changed();
    }

    void clear()
    {
        m_waveform.clear();
        emit changed();
    }

signals:
    void changed();

private:
    Waveform m_waveform;
};

} // namespace piejam::gui::model
