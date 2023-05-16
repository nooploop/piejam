// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QQuickItem>

#include <memory>

namespace piejam::gui::item
{

class Scope : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* lines READ lines WRITE
                       setLines NOTIFY linesChanged FINAL)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);
    ~Scope();

    auto lines() const noexcept -> model::ScopeLinesObject*;
    void setLines(model::ScopeLinesObject*);

    auto color() const noexcept -> QColor const&;
    void setColor(QColor const&);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void linesChanged();
    void colorChanged();

private:
    void updateTransformMatrix(QSGTransformNode&);
    auto updateGeometry(
            QSGNode*,
            model::ScopeLinesObject const*,
            QColor const&,
            bool& dirtyGeometry,
            bool& dirtyMaterial) -> QSGNode*;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::item
