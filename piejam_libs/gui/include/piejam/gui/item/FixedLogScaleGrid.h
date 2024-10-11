// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

#include <QQuickItem>

namespace piejam::gui::item
{

class FixedLogScaleGrid : public QQuickItem
{
    Q_OBJECT

    M_PIEJAM_GUI_WRITABLE_PROPERTY(Qt::Orientation, orientation, setOrientation)
    M_PIEJAM_GUI_WRITABLE_PROPERTY(QColor, color, setColor)
    M_PIEJAM_GUI_WRITABLE_PROPERTY(QVector<double>, ticks, setTicks)

    QML_ELEMENT

public:
    explicit FixedLogScaleGrid(QQuickItem* parent = nullptr);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::item
