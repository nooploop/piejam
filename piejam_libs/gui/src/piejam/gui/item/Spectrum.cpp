// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Spectrum.h>

#include <piejam/functional/in_interval.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/math.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QVariant>

#include <boost/assert.hpp>

#include <array>
#include <cmath>
#include <span>

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

std::array s_frequencyTicks{10.f,   20.f,   30.f,   40.f,    50.f,   60.f,
                            70.f,   80.f,   90.f,   100.f,   200.f,  300.f,
                            400.f,  500.f,  600.f,  700.f,   800.f,  900.f,
                            1000.f, 2000.f, 3000.f, 4000.f,  5000.f, 6000.f,
                            7000.f, 8000.f, 9000.f, 10000.f, 20000.f};
std::array s_frequencyLabels{10.f, 100.f, 1000.f, 10000.f};

std::array s_levelTicks{-100.f, -80.f, -60.f, -40.f, -20.f, 0.f};
std::array s_levelLabels{-80.f, -60.f, -40.f, -20.f, 0.f};

} // namespace

struct Spectrum::Impl
{
    bool scaleIsDirty{true};
    bool spectrumDataADirty{true};
    bool spectrumDataBDirty{true};
    bool spectrumColorADirty{true};
    bool spectrumColorBDirty{true};

    FrequencyScale frequencyScale;
    decltype(s_frequencyTicks) frequencyPositions;
    QVariantList frequencyLabels;

    LevelScale levelScale;
    decltype(s_levelTicks) levelPositions;
    QVariantList levelLabels;

    model::SpectrumData* spectrumDataA{};
    QMetaObject::Connection spectrumDataAChangedConnection;
    std::vector<QPointF> spectrumA;
    model::SpectrumData* spectrumDataB{};
    QMetaObject::Connection spectrumDataBChangedConnection;
    std::vector<QPointF> spectrumB;

    QColor spectrumColorA{255, 0, 0};
    QColor spectrumColorB{0, 0, 255};

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
    , m_impl(std::make_unique<Impl>())
{
    setFlag(ItemHasContents);

    connect(this, &Spectrum::widthChanged, [this]() {
        m_impl->frequencyScale.size = static_cast<int>(width());
        std::ranges::transform(
                s_frequencyTicks,
                m_impl->frequencyPositions.begin(),
                [this](float const f) {
                    return m_impl->frequencyScale.frequencyToPosition(f);
                });
        m_impl->frequencyLabels.clear();
        std::ranges::transform(
                s_frequencyLabels,
                std::back_inserter(m_impl->frequencyLabels),
                [this](float const f) {
                    SpectrumScaleLabel r;
                    r.position = m_impl->frequencyScale.frequencyToPosition(f);
                    r.value = f;
                    return QVariant::fromValue(r);
                });
        emit frequencyLabelsChanged();
        m_impl->scaleIsDirty = true;
        m_impl->spectrumDataADirty = true;
        m_impl->spectrumDataBDirty = true;
        update();
    });

    connect(this, &Spectrum::heightChanged, [this]() {
        m_impl->levelScale.size = static_cast<int>(height());
        std::ranges::transform(
                s_levelTicks,
                m_impl->levelPositions.begin(),
                [this](float const level) {
                    return height() - m_impl->levelScale.levelToPosition(level);
                });
        m_impl->levelLabels.clear();
        std::ranges::transform(
                s_levelLabels,
                std::back_inserter(m_impl->levelLabels),
                [this](float const level) {
                    SpectrumScaleLabel r;
                    r.position = height() -
                                 m_impl->levelScale.levelToPosition(level);
                    r.value = level;
                    return QVariant::fromValue(r);
                });
        emit levelLabelsChanged();
        m_impl->scaleIsDirty = true;
        m_impl->spectrumDataADirty = true;
        m_impl->spectrumDataBDirty = true;
        update();
    });
}

Spectrum::~Spectrum() = default;

auto
Spectrum::levelLabels() const -> QVariantList
{
    return m_impl->levelLabels;
}

auto
Spectrum::spectrumAData() const noexcept -> model::SpectrumData*
{
    return m_impl->spectrumDataA;
}

void
Spectrum::setSpectrumAData(model::SpectrumData* const x)
{
    if (m_impl->spectrumDataA != x)
    {
        m_impl->spectrumDataA = x;
        emit spectrumADataChanged();

        if (m_impl->spectrumDataA)
        {
            m_impl->spectrumDataAChangedConnection = QObject::connect(
                    m_impl->spectrumDataA,
                    &piejam::gui::model::SpectrumData::changed,
                    this,
                    [this]() {
                        m_impl->spectrumA = m_impl->calcSpectrum(
                                m_impl->spectrumDataA->get(),
                                height());
                        m_impl->spectrumDataADirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->spectrumDataAChangedConnection);
        }

        m_impl->spectrumA.clear();
        m_impl->spectrumDataADirty = true;
        update();
    }
}

auto
Spectrum::spectrumBData() const noexcept -> model::SpectrumData*
{
    return m_impl->spectrumDataB;
}

void
Spectrum::setSpectrumBData(model::SpectrumData* const x)
{
    if (m_impl->spectrumDataB != x)
    {
        m_impl->spectrumDataB = x;
        emit spectrumADataChanged();

        if (m_impl->spectrumDataB)
        {
            m_impl->spectrumDataBChangedConnection = QObject::connect(
                    m_impl->spectrumDataB,
                    &piejam::gui::model::SpectrumData::changed,
                    this,
                    [this]() {
                        m_impl->spectrumB = m_impl->calcSpectrum(
                                m_impl->spectrumDataB->get(),
                                height());
                        m_impl->spectrumDataBDirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->spectrumDataBChangedConnection);
        }

        m_impl->spectrumB.clear();
        m_impl->spectrumDataBDirty = true;
        update();
    }
}

auto
Spectrum::spectrumAColor() const noexcept -> QColor const&
{
    return m_impl->spectrumColorA;
}

void
Spectrum::setSpectrumAColor(QColor const& c)
{
    if (m_impl->spectrumColorA != c)
    {
        m_impl->spectrumColorA = c;
        emit spectrumAColorChanged();

        m_impl->spectrumColorADirty = true;
        update();
    }
}

auto
Spectrum::spectrumBColor() const noexcept -> QColor const&
{
    return m_impl->spectrumColorB;
}

void
Spectrum::setSpectrumBColor(QColor const& c)
{
    if (m_impl->spectrumColorB != c)
    {
        m_impl->spectrumColorB = c;
        emit spectrumBColorChanged();

        m_impl->spectrumColorBDirty = true;
        update();
    }
}

auto
Spectrum::frequencyLabels() const -> QVariantList
{
    return m_impl->frequencyLabels;
}

auto
Spectrum::updateDataNode(
        QSGNode* node,
        std::vector<QPointF> const& data,
        QColor const& color,
        bool& dirtyGeometryFlag,
        bool& dirtyMaterialFlag) -> QSGNode*
{
    QSGGeometryNode* geometryNode{};
    QSGGeometry* geometry{};
    QSGFlatColorMaterial* material{};

    int const dataSize = static_cast<int>(data.size());
    if (!node)
    {
        geometryNode = new QSGGeometryNode();
        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                dataSize);

        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        geometry->setLineWidth(2);
        geometryNode->setGeometry(geometry);
        geometryNode->setFlag(QSGNode::OwnsGeometry);

        material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        geometryNode->setMaterial(material);
        geometryNode->setFlag(QSGNode::OwnsMaterial);
    }
    else
    {
        geometryNode = static_cast<QSGGeometryNode*>(node);
        geometry = geometryNode->geometry();
        material = static_cast<QSGFlatColorMaterial*>(geometryNode->material());

        if (dataSize != geometry->vertexCount())
        {
            geometry->allocate(dataSize);
        }
    }

    if (dirtyGeometryFlag)
    {
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
        for (QPointF const& p : data)
        {
            vertices->set(p.x(), p.y());
            ++vertices;
        }

        geometryNode->markDirty(QSGNode::DirtyGeometry);

        dirtyGeometryFlag = false;
    }

    if (dirtyMaterialFlag)
    {
        material->setColor(color);

        geometryNode->markDirty(QSGNode::DirtyMaterial);

        dirtyMaterialFlag = false;
    }

    return geometryNode;
}

auto
Spectrum::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*)
        -> QSGNode*
{
    QSGGeometryNode* node{};
    QSGGeometry* geometry{};

    if (!oldNode)
    {
        node = new QSGGeometryNode();
        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                (m_impl->frequencyPositions.size() +
                 m_impl->levelPositions.size()) *
                        2);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 255, 255));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(updateDataNode(
                nullptr,
                m_impl->spectrumA,
                m_impl->spectrumColorA,
                m_impl->spectrumDataADirty,
                m_impl->spectrumColorADirty));
        node->appendChildNode(updateDataNode(
                nullptr,
                m_impl->spectrumB,
                m_impl->spectrumColorB,
                m_impl->spectrumDataBDirty,
                m_impl->spectrumColorBDirty));
    }
    else
    {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();

        auto dataANode = node->firstChild();
        BOOST_ASSERT(dataANode);
        BOOST_VERIFY(
                dataANode == updateDataNode(
                                     dataANode,
                                     m_impl->spectrumA,
                                     m_impl->spectrumColorA,
                                     m_impl->spectrumDataADirty,
                                     m_impl->spectrumColorADirty));

        auto dataBNode = dataANode->nextSibling();
        BOOST_ASSERT(dataBNode);
        BOOST_VERIFY(
                dataBNode == updateDataNode(
                                     dataBNode,
                                     m_impl->spectrumB,
                                     m_impl->spectrumColorB,
                                     m_impl->spectrumDataBDirty,
                                     m_impl->spectrumColorBDirty));
    }

    if (m_impl->scaleIsDirty)
    {
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

        for (std::size_t i = 0; i < m_impl->frequencyPositions.size();
             ++i, vertices += 2)
        {
            auto x = m_impl->frequencyPositions[i];
            vertices[0].x = x;
            vertices[0].y = 0;
            vertices[1].x = x;
            vertices[1].y = height() - 1;
        }

        for (std::size_t i = 0; i < m_impl->levelPositions.size();
             ++i, vertices += 2)
        {
            auto y = m_impl->levelPositions[i];
            vertices[0].x = 0;
            vertices[0].y = y;
            vertices[1].x = width() - 1;
            vertices[1].y = y;
        }

        node->markDirty(QSGNode::DirtyGeometry);
        m_impl->scaleIsDirty = false;
    }

    return node;
}

} // namespace piejam::gui::item
