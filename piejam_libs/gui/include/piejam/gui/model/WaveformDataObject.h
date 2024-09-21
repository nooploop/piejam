// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/WaveformData.h>

#include <QObject>

namespace piejam::gui::model
{

class WaveformDataObject final : public QObject
{
    Q_OBJECT

public:
    auto get() noexcept -> WaveformData&
    {
        return m_lines;
    }

    auto get() const noexcept -> WaveformData const&
    {
        return m_lines;
    }

    void update()
    {
        emit changed();
    }

signals:
    void changed();

private:
    WaveformData m_lines;
};

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::WaveformDataObject*)
