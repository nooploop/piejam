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

    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* linesA READ linesA WRITE
                       setLinesA NOTIFY linesAChanged FINAL)

    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* linesB READ linesB WRITE
                       setLinesB NOTIFY linesBChanged FINAL)

    Q_PROPERTY(QColor linesAColor READ linesAColor WRITE setLinesAColor NOTIFY
                       linesAColorChanged FINAL)

    Q_PROPERTY(QColor linesBColor READ linesBColor WRITE setLinesBColor NOTIFY
                       linesBColorChanged FINAL)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);
    ~Scope();

    auto linesA() const noexcept -> model::ScopeLinesObject*;
    void setLinesA(model::ScopeLinesObject*);

    auto linesB() const noexcept -> model::ScopeLinesObject*;
    void setLinesB(model::ScopeLinesObject*);

    auto linesAColor() const noexcept -> QColor const&;
    void setLinesAColor(QColor const&);

    auto linesBColor() const noexcept -> QColor const&;
    void setLinesBColor(QColor const&);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void linesAChanged();
    void linesBChanged();
    void linesAColorChanged();
    void linesBColorChanged();

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
