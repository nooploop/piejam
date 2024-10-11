// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/FixedLinearScaleGrid.h>

#include <piejam/math.h>

#include <QGuiApplication>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QScreen>

#include <boost/polymorphic_cast.hpp>

namespace piejam::gui::item
{

struct FixedLinearScaleGrid::Impl
{
    bool scaleIsDirty{};
    bool colorIsDirty{};
};

FixedLinearScaleGrid::FixedLinearScaleGrid(QQuickItem* parent)
    : QQuickItem(parent)
    , m_orientation{Qt::Orientation::Horizontal}
    , m_color{255, 255, 255}
    , m_ticks{0.0, 1.0}
    , m_impl{make_pimpl<Impl>()}
{
    setFlag(ItemHasContents);

    connect(this, &FixedLinearScaleGrid::widthChanged, this, [this]() {
        m_impl->scaleIsDirty = true;
        update();
    });

    connect(this, &FixedLinearScaleGrid::heightChanged, this, [this]() {
        m_impl->scaleIsDirty = true;
        update();
    });

    connect(this, &FixedLinearScaleGrid::colorChanged, this, [this]() {
        m_impl->colorIsDirty = true;
        update();
    });

    connect(this, &FixedLinearScaleGrid::ticksChanged, this, [this]() {
        BOOST_ASSERT(m_ticks.size() >= 2);
        m_impl->scaleIsDirty = true;
        update();
    });
}

auto
FixedLinearScaleGrid::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
        -> QSGNode*
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
                m_ticks.size() * 2); // x, y
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

        auto const min = m_ticks.front();
        auto const max = m_ticks.back();
        auto const diff = max - min;

        // ((v - src_lo) / (src_hi - src_lo)) * (dst_hi - dst_lo) + dst_lo

        auto const w = width();
        auto const h = height();
        if (m_orientation == Qt::Orientation::Horizontal)
        {
            for (auto tick : qAsConst(m_ticks))
            {
                auto const x = ((tick - min) / diff) * w;
                vertices[0].x = x;
                vertices[0].y = 0;
                vertices[1].x = x;
                vertices[1].y = h;

                vertices += 2; // start, end
            }
        }
        else
        {
            for (auto tick : qAsConst(m_ticks))
            {
                auto const y = ((tick - min) / diff) * h;
                vertices[0].x = 0;
                vertices[0].y = y;
                vertices[1].x = w;
                vertices[1].y = y;

                vertices += 2; // start, end
            }
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
