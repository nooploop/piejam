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
                m_lines->lines().size() * 2);
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
        geometry->allocate(m_lines->lines().size() * 2);
    }

    auto setVertex = [height = size().height()](auto& v, auto const& p) {
        v.set(p.x(), ((std::clamp(-p.y(), -1., 1.) + 1) / 2.f) * height);
    };

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (int i = 0, e = m_lines->lines().size(); i < e; ++i)
    {
        setVertex(vertices[i * 2], m_lines->lines()[i].p1());
        setVertex(vertices[i * 2 + 1], m_lines->lines()[i].p2());
    }

    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}

void
Scope::setLines(model::ScopeLines* x)
{
    if (m_lines != x)
    {
        m_lines = x;
        emit linesChanged();
        update();

        m_linesChangedConnection =
                connect(m_lines,
                        &model::ScopeLines::linesChanged,
                        this,
                        &Scope::update);
    }
}

} // namespace piejam::gui::item
