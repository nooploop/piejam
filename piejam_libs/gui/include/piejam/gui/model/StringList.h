// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

namespace piejam::gui::model
{

class StringList final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList elements READ elements NOTIFY elementsChanged FINAL)
    Q_PROPERTY(int focused READ focused NOTIFY focusedChanged FINAL)

public:
    auto elements() const -> QStringList const& { return m_elements; }
    void setElements(QStringList const& x)
    {
        if (m_elements != x)
        {
            m_elements = x;
            emit elementsChanged();
            emit focusedChanged();
        }
    }

    auto focused() const -> int { return m_focused; }
    void setFocused(int const x)
    {
        if (m_focused != x)
        {
            m_focused = x;
            emit focusedChanged();
        }
    }

signals:

    void elementsChanged();
    void focusedChanged();

private:
    QStringList m_elements;
    int m_focused{-1};
};

} // namespace piejam::gui::model
