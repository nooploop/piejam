// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

#include <QQuickItem>

namespace piejam::gui::item
{

class Scope : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)

    Q_PROPERTY(piejam::gui::model::ScopeData* scopeData READ scopeData WRITE
                       setScopeData NOTIFY scopeDataChanged FINAL)

    Q_PROPERTY(float peakLevel READ peakLevel NOTIFY peakLevelChanged FINAL)

    QML_ELEMENT

public:
    Scope(QQuickItem* parent = nullptr);

    auto color() const noexcept -> QColor const&;
    void setColor(QColor const&);

    auto scopeData() const noexcept -> model::ScopeData*;
    void setScopeData(model::ScopeData*);

    auto peakLevel() const noexcept -> float;
    Q_INVOKABLE void syncPeakLevel(float otherPeakLevel);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void colorChanged();
    void scopeDataChanged();
    void peakLevelChanged();

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::item
