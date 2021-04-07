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

    if (!oldNode)
    {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                m_lines->get().size() * 2);
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
        geometry->allocate(m_lines->get().size() * 2);
    }

    auto setVertex = [height = size().height() / 2.f](
                             QSGGeometry::Point2D& v,
                             float const x,
                             float const y) { v.set(x, (y + 1) * height); };

    float const* y0 = m_lines->get().y0().data();
    float const* y1 = m_lines->get().y1().data();

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0, e = m_lines->get().size(); i < e;
         ++i, ++y0, ++y1, vertices += 2)
    {
        setVertex(vertices[0], i, *y0);
        setVertex(vertices[1], i, *y1);
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

        m_linesChangedConnection =
                connect(m_lines,
                        &model::ScopeLinesObject::changed,
                        this,
                        &Scope::update);
    }
}

} // namespace piejam::gui::item
