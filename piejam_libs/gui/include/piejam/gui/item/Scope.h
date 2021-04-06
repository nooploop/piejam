// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ScopeLines.h>

#include <QQuickItem>

#include <vector>

namespace piejam::gui::item
{

class Scope : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::ScopeLines* lines READ lines WRITE setLines
                       NOTIFY linesChanged)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);

    auto lines() noexcept -> model::ScopeLines* { return m_lines; }
    void setLines(model::ScopeLines*);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void linesChanged();

private:
    model::ScopeLines* m_lines{};
    QMetaObject::Connection m_linesChangedConnection;
};

} // namespace piejam::gui::item
