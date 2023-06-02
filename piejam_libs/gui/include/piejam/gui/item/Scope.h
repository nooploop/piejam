// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
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

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)

    Q_PROPERTY(piejam::gui::model::ScopeData* scopeData READ scopeData WRITE
                       setScopeData NOTIFY scopeDataChanged FINAL)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);
    ~Scope() override;

    auto color() const noexcept -> QColor const&;
    void setColor(QColor const&);

    auto scopeData() const noexcept -> model::ScopeData*;
    void setScopeData(model::ScopeData*);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void colorChanged();
    void scopeDataChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::item
