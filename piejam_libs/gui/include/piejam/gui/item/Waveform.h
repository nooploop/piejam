// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>

#include <QQuickItem>

namespace piejam::gui::item
{

class Waveform : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformData READ
                       waveformData WRITE setWaveformData NOTIFY
                               waveformDataChanged FINAL)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)

    QML_ELEMENT

public:
    Waveform(QQuickItem* parent = nullptr);

    auto waveformData() const noexcept -> model::WaveformDataObject*;
    void setWaveformData(model::WaveformDataObject*);

    auto color() const noexcept -> QColor const&;
    void setColor(QColor const&);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void waveformDataChanged();
    void colorChanged();

private:
    void updateTransformMatrix(QSGTransformNode&);

    struct Impl;
    pimpl<Impl> m_impl;
};

} // namespace piejam::gui::item
