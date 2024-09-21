// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/DbScale.h>

#include <piejam/gui/model/DbScaleData.h>

#include <QGuiApplication>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QScreen>

#include <boost/polymorphic_cast.hpp>

namespace piejam::gui::item
{

struct DbScale::Impl
{
    QPointer<model::DbScaleData> scaleData{};

    bool scaleIsDirty{};
    bool colorIsDirty{};
};

DbScale::DbScale(QQuickItem* parent)
    : QQuickItem(parent)
    , m_impl(std::make_unique<Impl>())
{
    setFlag(ItemHasContents);

    connect(this, &DbScale::widthChanged, [this]() {
        m_impl->scaleIsDirty = true;
        update();
    });

    connect(this, &DbScale::heightChanged, [this]() {
        m_impl->scaleIsDirty = true;
        update();
    });
}

DbScale::~DbScale() = default;

auto
DbScale::scaleData() const noexcept -> model::DbScaleData*
{
    return m_impl->scaleData;
}

void
DbScale::setScaleData(model::DbScaleData* const scaleData)
{
    if (m_impl->scaleData != scaleData)
    {
        m_impl->scaleData = scaleData;
        m_impl->scaleIsDirty = true;
        emit scaleDataChanged();
    }
}

void
DbScale::setColor(QColor const& x)
{
    if (m_color != x)
    {
        m_color = x;
        m_impl->colorIsDirty = true;
        emit colorChanged();
        update();
    }
}

void
DbScale::setTickOffset(int const x)
{
    if (m_tickOffset != x)
    {
        m_tickOffset = x;
        m_impl->scaleIsDirty = true;
        emit tickOffsetChanged();
        update();
    }
}

void
DbScale::setEdge(Edge const x)
{
    if (m_edge != x)
    {
        m_edge = x;
        m_impl->scaleIsDirty = true;
        emit edgeChanged();
        update();
    }
}

auto
DbScale::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) -> QSGNode*
{
    QSGGeometryNode* node{};
    QSGGeometry* geometry{};
    QSGFlatColorMaterial* material{};

    auto const devicePixelRatio = qGuiApp->devicePixelRatio();

    if (!oldNode)
    {
        node = new QSGGeometryNode();
        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                (m_impl->scaleData ? m_impl->scaleData->ticks().size() : 0) *
                                2 +
                        2);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        geometry->setLineWidth(devicePixelRatio);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }
    else
    {
        node = boost::polymorphic_downcast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        material = boost::polymorphic_downcast<QSGFlatColorMaterial*>(
                node->material());
    }

    if (m_impl->scaleIsDirty)
    {
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

        auto const edgeX = (m_edge == Edge::Left ? 0.5 : width() - 0.5);
        vertices[0].x = edgeX;
        vertices[0].y = 0;
        vertices[1].x = edgeX;
        vertices[1].y = height();
        vertices += 2;

        // ticks
        auto const ticks = m_impl->scaleData ? m_impl->scaleData->ticks()
                                             : QVector<model::DbScaleTick>{};
        auto const offset = m_tickOffset;
        auto const h = height() - 2 * offset;
        for (int i = 0, e = ticks.size(); i < e; ++i, vertices += 2)
        {
            auto y = (1 - ticks[i].position) * h + offset;
            vertices[0].x = 0.5;
            vertices[0].y = y;
            vertices[1].x = width() - 0.5;
            vertices[1].y = y;
        }

        node->markDirty(QSGNode::DirtyGeometry);
        m_impl->scaleIsDirty = false;
    }

    if (m_impl->colorIsDirty)
    {
        material->setColor(m_color);

        node->markDirty(QSGNode::DirtyMaterial);
        m_impl->colorIsDirty = false;
    }

    return node;
}

} // namespace piejam::gui::item
