// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

#include <QQuickItem>
#include <QVariantList>

namespace piejam::gui::item
{

class Spectrum : public QQuickItem
{
    Q_OBJECT

    M_PIEJAM_GUI_WRITABLE_PROPERTY(
            piejam::gui::model::SpectrumSlot*,
            spectrum,
            setSpectrum)
    M_PIEJAM_GUI_WRITABLE_PROPERTY(QColor, color, setColor)

    QML_ELEMENT

public:
    Spectrum(QQuickItem* parent = nullptr);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

private:
    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::item
