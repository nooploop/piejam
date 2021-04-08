// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/ScopeLinesObject.h>

#include <QQuickItem>

#include <vector>

namespace piejam::gui::item
{

class Scope : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* lines READ lines WRITE
                       setLines NOTIFY linesChanged FINAL)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);

    auto lines() noexcept -> model::ScopeLinesObject* { return m_lines; }
    void setLines(model::ScopeLinesObject*);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void linesChanged();

private:
    void updateTransformMatrix(QSGTransformNode&);
    auto updateGeometry(QSGNode*) -> QSGNode*;

    model::ScopeLinesObject* m_lines{};

    QMetaObject::Connection m_linesChangedConnection;
    bool m_updateTransformMatrix{};
};

} // namespace piejam::gui::item
