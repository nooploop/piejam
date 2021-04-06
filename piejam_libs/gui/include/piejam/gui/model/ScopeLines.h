// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QLineF>
#include <QObject>
#include <QVector>

namespace piejam::gui::model
{

class ScopeLines final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
            QVector<QLineF> lines READ lines WRITE setLines NOTIFY linesChanged)

public:
    auto lines() const noexcept -> QVector<QLineF> const& { return m_lines; }
    void setLines(QVector<QLineF> const& x)
    {
        if (m_lines != x)
        {
            m_lines = x;
            emit linesChanged();
        }
    }

signals:
    void linesChanged();

private:
    QVector<QLineF> m_lines;
};

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::ScopeLines*)
