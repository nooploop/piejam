// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/DbScaleTick.h>

#include <QObject>
#include <QVector>

namespace piejam::gui::model
{

class DbScaleData : public QObject
{
    Q_OBJECT

    M_PIEJAM_GUI_CONSTANT_PROPERTY(
            QVector<piejam::gui::model::DbScaleTick>,
            ticks)
public:
    DbScaleData() noexcept = default;
    DbScaleData(QVector<DbScaleTick> ticks) noexcept
        : m_ticks(std::move(ticks))
    {
    }

    Q_INVOKABLE float dBToPosition(float const dB) const;

    Q_INVOKABLE float dBAt(float const position) const;

private:
    QVector<DbScaleTick> m_ticks;
};

} // namespace piejam::gui::model
