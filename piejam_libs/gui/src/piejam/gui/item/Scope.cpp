// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <QQuickWindow>
#include <QSGFlatColorMaterial>

namespace piejam::gui::item
{

Scope::Scope(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
}

auto
Scope::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*) -> QSGNode*
{
    if (!m_lines)
        return nullptr;

    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;

    auto setVertexX = [](QSGGeometry::Point2D* vertices, int vertexCount) {
        for (int i = 0, e = vertexCount / 2; i < e; ++i, vertices += 2)
        {
            vertices[0].x = i;
            vertices[1].x = i;
        }
    };

    if (!oldNode)
    {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                m_lines->get().size() * 2);
        setVertexX(geometry->vertexDataAsPoint2D(), geometry->vertexCount());
        geometry->setLineWidth(1);
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 255, 255));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    }
    else
    {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        if (static_cast<std::size_t>(geometry->vertexCount()) !=
            m_lines->get().size() * 2)
        {
            geometry->allocate(m_lines->get().size() * 2);
            setVertexX(
                    geometry->vertexDataAsPoint2D(),
                    geometry->vertexCount());
        }
    }

    auto setVertexY = [height = size().height() /
                                2.f](QSGGeometry::Point2D& v, float const y) {
        v.y = (1 - y) * height;
    };

    float const* yData = m_lines->get().ys().data();

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0, e = m_lines->get().size(); i < e;
         ++i, yData += 2, vertices += 2)
    {
        setVertexY(vertices[0], yData[0]);
        setVertexY(vertices[1], yData[1]);
    }

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void
Scope::setLines(model::ScopeLinesObject* x)
{
    if (m_lines != x)
    {
        m_lines = x;
        emit linesChanged();
        update();

        if (m_lines)
        {
            m_linesChangedConnection =
                    connect(m_lines,
                            &model::ScopeLinesObject::changed,
                            this,
                            &Scope::update);
        }
        else
        {
            m_linesChangedConnection = {};
        }
    }
}

} // namespace piejam::gui::item
