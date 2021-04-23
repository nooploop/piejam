// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
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
            piejam::gui::model::SpectrumData* spectrumDataA READ spectrumDataA
                    WRITE setSpectrumDataA NOTIFY spectrumDataAChanged FINAL)
    Q_PROPERTY(
            piejam::gui::model::SpectrumData* spectrumDataB READ spectrumDataB
                    WRITE setSpectrumDataB NOTIFY spectrumDataBChanged FINAL)

    QML_ELEMENT

public:
    Spectrum(QQuickItem* parent = nullptr);
    ~Spectrum();

    auto levelLabels() const -> QVariantList;
    auto frequencyLabels() const -> QVariantList;

    auto spectrumDataA() const noexcept -> model::SpectrumData*;
    void setSpectrumDataA(model::SpectrumData*);

    auto spectrumDataB() const noexcept -> model::SpectrumData*;
    void setSpectrumDataB(model::SpectrumData*);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void levelLabelsChanged();
    void frequencyLabelsChanged();
    void spectrumDataAChanged();
    void spectrumDataBChanged();

private:
    auto updateDataNode(QSGNode*, std::vector<QPointF> const&, bool& dirtyFlag)
            -> QSGNode*;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::item

Q_DECLARE_METATYPE(piejam::gui::item::SpectrumScaleLabel)
