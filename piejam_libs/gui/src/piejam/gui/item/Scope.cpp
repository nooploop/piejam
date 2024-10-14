// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <piejam/gui/model/ScopeSlot.h>

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

        int const dataSize = scope ? static_cast<int>(scope->get().size()) : 0;
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

        if (scopeDirty)
        {
            if (scope)
            {
                QSGGeometry::Point2D* vertices =
                        geometry->vertexDataAsPoint2D();
                for (std::size_t n = 0; n < scope->get().size(); ++n)
                {
                    vertices->set(static_cast<float>(n), scope->get()[n]);
                    ++vertices;
                }
            }

            geometryNode->markDirty(QSGNode::DirtyGeometry);

            scopeDirty = false;
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
    bool scopeDirty{true};
    model::ScopeSlot* scope{};
    float peakLevel{0.f};
    std::optional<float> syncedPeakLevel;
    QMetaObject::Connection scopeChangedConnection;
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
Scope::scope() const noexcept -> model::ScopeSlot*
{
    return m_impl->scope;
}

void
Scope::setScope(model::ScopeSlot* const x)
{
    if (m_impl->scope != x)
    {
        m_impl->scope = x;
        emit scopeChanged();

        if (m_impl->scope)
        {
            m_impl->scopeChangedConnection = QObject::connect(
                    m_impl->scope,
                    &piejam::gui::model::ScopeSlot::changed,
                    this,
                    [this]() {
                        m_impl->scopeDirty = true;

                        float newPeakLevel = math::flush_to_zero_if(
                                std::transform_reduce(
                                        m_impl->scope->get().begin(),
                                        m_impl->scope->get().end(),
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
            QObject::disconnect(m_impl->scopeChangedConnection);
        }

        m_impl->scopeDirty = true;
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
