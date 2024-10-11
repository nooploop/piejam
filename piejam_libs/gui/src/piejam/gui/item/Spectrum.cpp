// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Spectrum.h>

#include <piejam/gui/model/SpectrumData.h>

#include <piejam/functional/in_interval.h>
#include <piejam/math.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QVariant>

#include <boost/assert.hpp>
#include <boost/polymorphic_cast.hpp>

#include <cmath>

namespace piejam::gui::item
{

namespace
{

struct FrequencyScale final
{
    float size{1};
    static constexpr float minFrequency{10};
    static constexpr float maxFrequency{20000};

    static constexpr bool inRange(float const f) noexcept
    {
        return in_closed(f, minFrequency, maxFrequency);
    }

    constexpr auto frequencyToPosition(float const f) noexcept -> float
    {
        constexpr auto minLog10 = std::log10(minFrequency);
        constexpr auto maxLog10 = std::log10(maxFrequency);
        constexpr auto diffLog10 = maxLog10 - minLog10;

        return ((std::log10(math::clamp(f, minFrequency, maxFrequency)) -
                 minLog10) /
                diffLog10) *
               size;
    }
};

struct LevelScale final
{
    float size{1};
    static constexpr float minLevel{-100};
    static constexpr float maxLevel{0};

    static constexpr bool inRange(float const level) noexcept
    {
        return in_closed(level, minLevel, maxLevel);
    }

    constexpr auto levelToPosition(float const level) noexcept -> float
    {
        constexpr auto diff = maxLevel - minLevel;
        return ((math::clamp(level, minLevel, maxLevel) - minLevel) / diff) *
               size;
    }
};

} // namespace

struct Spectrum::Impl
{
    bool spectrumDataDirty{true};
    bool colorDirty{true};

    FrequencyScale frequencyScale;
    LevelScale levelScale;

    QMetaObject::Connection spectrumDataChangedConnection;
    std::vector<QPointF> spectrumPoints;

    auto calcSpectrum(
            piejam::gui::model::SpectrumDataPoints const& dataPoints,
            float const height)
    {
        std::vector<QPointF> result;

        for (auto const& dataPoint : dataPoints)
        {
            result.emplace_back(
                    frequencyScale.frequencyToPosition(dataPoint.frequency_Hz),
                    height - levelScale.levelToPosition(dataPoint.level_dB));
        }

        return result;
    }
};

Spectrum::Spectrum(QQuickItem* parent)
    : QQuickItem(parent)
    , m_impl{make_pimpl<Impl>()}
{
    setFlag(ItemHasContents);

    connect(this, &Spectrum::widthChanged, [this]() {
        m_impl->frequencyScale.size = static_cast<int>(width());
        m_impl->spectrumDataDirty = true;
        update();
    });

    connect(this, &Spectrum::heightChanged, [this]() {
        m_impl->levelScale.size = static_cast<int>(height());
        m_impl->spectrumDataDirty = true;
        update();
    });

    connect(this, &Spectrum::spectrumDataChanged, this, [this]() {
        if (m_spectrumData)
        {
            m_impl->spectrumDataChangedConnection = QObject::connect(
                    m_spectrumData,
                    &piejam::gui::model::SpectrumData::changed,
                    this,
                    [this]() {
                        m_impl->spectrumPoints = m_impl->calcSpectrum(
                                m_spectrumData->get(),
                                height());
                        m_impl->spectrumDataDirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->spectrumDataChangedConnection);
        }

        m_impl->spectrumPoints.clear();
        m_impl->spectrumDataDirty = true;
        update();
    });
}

auto
Spectrum::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*)
        -> QSGNode*
{
    QSGGeometryNode* node{};
    QSGGeometry* geometry{};
    QSGFlatColorMaterial* material{};

    int const dataSize = static_cast<int>(m_impl->spectrumPoints.size());
    if (!oldNode)
    {
        node = new QSGGeometryNode();
        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                dataSize);

        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        geometry->setLineWidth(2);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }
    else
    {
        node = boost::polymorphic_downcast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        material = boost::polymorphic_downcast<QSGFlatColorMaterial*>(
                node->material());

        if (dataSize != geometry->vertexCount())
        {
            geometry->allocate(dataSize);
        }
    }

    if (m_impl->spectrumDataDirty)
    {
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
        for (QPointF const& p : m_impl->spectrumPoints)
        {
            vertices->set(p.x(), p.y());
            ++vertices;
        }

        node->markDirty(QSGNode::DirtyGeometry);

        m_impl->spectrumDataDirty = false;
    }

    if (m_impl->colorDirty)
    {
        material->setColor(m_color);

        node->markDirty(QSGNode::DirtyMaterial);

        m_impl->colorDirty = false;
    }

    return node;
}

} // namespace piejam::gui::item
