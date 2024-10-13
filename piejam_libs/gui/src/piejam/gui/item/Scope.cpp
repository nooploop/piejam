// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <piejam/gui/model/ScopeData.h>

#include <piejam/functional/operators.h>
#include <piejam/math.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>

#include <boost/polymorphic_cast.hpp>

#include <numeric>
#include <ranges>

namespace piejam::gui::item
{

struct Scope::Impl
{
    void updateTransformMatrix(QSGTransformNode& node, qreal height)
    {
        if (transformMatrixDirty)
        {
            QMatrix4x4 m;

            float modY = syncedPeakLevel ? *syncedPeakLevel : peakLevel;
            if (modY == 0.f)
            {
                modY = 1.f;
            }

            m.scale(1, static_cast<float>(height / (-2. * modY)));
            m.translate(0, -modY);
            node.setMatrix(m);

            node.markDirty(QSGNode::DirtyMatrix);

            transformMatrixDirty = false;
        }
    }

    auto updateGeometry(QSGGeometryNode* const oldNode) -> QSGGeometryNode*
    {
        QSGGeometryNode* geometryNode{};
        QSGGeometry* geometry{};
        QSGFlatColorMaterial* material{};

        int const dataSize =
                scopeData ? static_cast<int>(scopeData->get().size()) : 0;
        if (oldNode)
        {
            geometryNode =
                    boost::polymorphic_downcast<QSGGeometryNode*>(oldNode);
            geometry = geometryNode->geometry();
            material = static_cast<QSGFlatColorMaterial*>(
                    geometryNode->material());

            if (dataSize != geometry->vertexCount())
            {
                geometry->allocate(dataSize);
            }
        }
        else
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
            material->setColor(color);
            geometryNode->setMaterial(material);
            geometryNode->setFlag(QSGNode::OwnsMaterial);
        }

        if (scopeDataDirty)
        {
            if (scopeData)
            {
                QSGGeometry::Point2D* vertices =
                        geometry->vertexDataAsPoint2D();
                for (std::size_t n = 0; n < scopeData->get().size(); ++n)
                {
                    vertices->set(static_cast<float>(n), scopeData->get()[n]);
                    ++vertices;
                }
            }

            geometryNode->markDirty(QSGNode::DirtyGeometry);

            scopeDataDirty = false;
        }

        if (colorDirty)
        {
            material->setColor(color);

            geometryNode->markDirty(QSGNode::DirtyMaterial);

            colorDirty = false;
        }

        return geometryNode;
    }

    QColor color{};
    bool colorDirty{true};
    bool transformMatrixDirty{true};
    bool scopeDataDirty{true};
    model::ScopeData* scopeData{};
    float peakLevel{0.f};
    std::optional<float> syncedPeakLevel;
    QMetaObject::Connection scopeDataChangedConnection;
};

Scope::Scope(QQuickItem* parent)
    : QQuickItem(parent)
    , m_impl{make_pimpl<Impl>()}
{
    setFlag(ItemHasContents);

    connect(this, &Scope::heightChanged, [this]() {
        m_impl->transformMatrixDirty = true;
        update();
    });
}

auto
Scope::color() const noexcept -> QColor const&
{
    return m_impl->color;
}

void
Scope::setColor(QColor const& c)
{
    if (m_impl->color != c)
    {
        m_impl->color = c;
        emit colorChanged();

        m_impl->colorDirty = true;
        update();
    }
}

auto
Scope::scopeData() const noexcept -> model::ScopeData*
{
    return m_impl->scopeData;
}

void
Scope::setScopeData(model::ScopeData* const x)
{
    if (m_impl->scopeData != x)
    {
        m_impl->scopeData = x;
        emit scopeDataChanged();

        if (m_impl->scopeData)
        {
            m_impl->scopeDataChangedConnection = QObject::connect(
                    m_impl->scopeData,
                    &piejam::gui::model::ScopeData::changed,
                    this,
                    [this]() {
                        m_impl->scopeDataDirty = true;

                        float newPeakLevel = math::flush_to_zero_if(
                                std::transform_reduce(
                                        m_impl->scopeData->get().begin(),
                                        m_impl->scopeData->get().end(),
                                        0.f,
                                        std::ranges::max,
                                        math::abs),
                                less(0.001f)); // -60 dB

                        if (m_impl->peakLevel != newPeakLevel)
                        {
                            m_impl->peakLevel = newPeakLevel;
                            emit peakLevelChanged();

                            if (!m_impl->syncedPeakLevel)
                            {
                                m_impl->transformMatrixDirty = true;
                            }
                        }

                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->scopeDataChangedConnection);
        }

        m_impl->scopeDataDirty = true;
        update();
    }
}

auto
Scope::peakLevel() const noexcept -> float
{
    return m_impl->peakLevel;
}

void
Scope::syncPeakLevel(float otherPeakLevel)
{
    if (m_impl->syncedPeakLevel != otherPeakLevel)
    {
        m_impl->syncedPeakLevel = otherPeakLevel;
        m_impl->transformMatrixDirty = true;

        update();
    }
}

auto
Scope::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*) -> QSGNode*
{
    QSGTransformNode* node{};

    if (oldNode)
    {
        node = boost::polymorphic_downcast<QSGTransformNode*>(oldNode);
        m_impl->updateTransformMatrix(*node, size().height());

        auto* geometryNode = node->firstChild();
        BOOST_ASSERT(geometryNode);
        BOOST_VERIFY(
                geometryNode ==
                m_impl->updateGeometry(
                        boost::polymorphic_downcast<QSGGeometryNode*>(
                                geometryNode)));
    }
    else
    {
        node = new QSGTransformNode();
        m_impl->updateTransformMatrix(*node, size().height());

        node->appendChildNode(m_impl->updateGeometry(nullptr));
    }

    return node;
}

} // namespace piejam::gui::item
