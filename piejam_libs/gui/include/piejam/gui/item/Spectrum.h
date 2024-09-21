// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QQuickItem>
#include <QVariantList>

#include <memory>

namespace piejam::gui::item
{

struct SpectrumScaleLabel
{
    Q_GADGET

    Q_PROPERTY(float position MEMBER position)
    Q_PROPERTY(float value MEMBER value)

public:
    float position{};
    float value{};
};

class Spectrum : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QVariantList levelLabels READ levelLabels NOTIFY
                       levelLabelsChanged FINAL)
    Q_PROPERTY(QVariantList frequencyLabels READ frequencyLabels NOTIFY
                       frequencyLabelsChanged FINAL)

    Q_PROPERTY(
            piejam::gui::model::SpectrumData* spectrumAData READ spectrumAData
                    WRITE setSpectrumAData NOTIFY spectrumADataChanged FINAL)
    Q_PROPERTY(
            piejam::gui::model::SpectrumData* spectrumBData READ spectrumBData
                    WRITE setSpectrumBData NOTIFY spectrumBDataChanged FINAL)

    Q_PROPERTY(QColor spectrumAColor READ spectrumAColor WRITE setSpectrumAColor
                       NOTIFY spectrumAColorChanged FINAL)

    Q_PROPERTY(QColor spectrumBColor READ spectrumBColor WRITE setSpectrumBColor
                       NOTIFY spectrumBColorChanged FINAL)

    QML_ELEMENT

public:
    Spectrum(QQuickItem* parent = nullptr);
    ~Spectrum();

    auto levelLabels() const -> QVariantList;
    auto frequencyLabels() const -> QVariantList;

    auto spectrumAData() const noexcept -> model::SpectrumData*;
    void setSpectrumAData(model::SpectrumData*);

    auto spectrumBData() const noexcept -> model::SpectrumData*;
    void setSpectrumBData(model::SpectrumData*);

    auto spectrumAColor() const noexcept -> QColor const&;
    void setSpectrumAColor(QColor const&);

    auto spectrumBColor() const noexcept -> QColor const&;
    void setSpectrumBColor(QColor const&);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void levelLabelsChanged();
    void frequencyLabelsChanged();
    void spectrumADataChanged();
    void spectrumBDataChanged();
    void spectrumAColorChanged();
    void spectrumBColorChanged();

private:
    auto updateDataNode(
            QSGNode*,
            std::vector<QPointF> const&,
            QColor const&,
            bool& dirtyGeometryFlag,
            bool& dirtyMaterialFlag) -> QSGNode*;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::item

Q_DECLARE_METATYPE(piejam::gui::item::SpectrumScaleLabel)
