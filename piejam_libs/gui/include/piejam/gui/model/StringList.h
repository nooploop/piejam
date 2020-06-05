// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
    StringList(QObject* parent = nullptr);

    auto elements() const -> QStringList const& { return m_elements; }
    void setElements(QStringList const&);

    auto focused() const -> int { return m_focused; }
    void setFocused(int);

signals:

    void elementsChanged();
    void focusedChanged();

private:
    QStringList m_elements;
    int m_focused{-1};
};

} // namespace piejam::gui::model
