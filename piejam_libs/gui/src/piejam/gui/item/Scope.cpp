// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <piejam/gui/model/ScopeData.h>

#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>

#include <boost/polymorphic_cast.hpp>

namespace piejam::gui::item
{

struct Scope::Impl
{
    void updateTransformMatrix(QSGTransformNode& node, qreal height)
    {
        if (transformMatrixDirty)
        {
            QMatrix4x4 m;
            m.scale(1, static_cast<float>(height / -2.));
            m.translate(0, -1);
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
    QMetaObject::Connection scopeDataChangedConnection;
};

Scope::Scope(QQuickItem* parent)
    : QQuickItem(parent)
    , m_impl(std::make_unique<Impl>())
{
    setFlag(ItemHasContents);

    connect(this, &Scope::heightChanged, [this]() {
        m_impl->transformMatrixDirty = true;
        update();
    });
}

Scope::~Scope() = default;

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
