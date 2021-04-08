// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <QQuickWindow>
#include <QSGFlatColorMaterial>
#include <QSGMaterial>

namespace piejam::gui::item
{

namespace
{

class ScopePointMaterial : public QSGMaterial
{
public:
    auto type() const noexcept -> QSGMaterialType* override;

    auto createShader() const -> QSGMaterialShader* override;

    void setColor(QColor const& color)
    {
        m_color = color;
        setFlag(Blending, m_color.alpha() != 0xff);
    }

    auto color() const noexcept -> QColor const& { return m_color; }

    auto compare(QSGMaterial const* const other) const noexcept -> int override
    {
        auto const material = static_cast<ScopePointMaterial const*>(other);
        return m_color.rgba() - material->color().rgba();
    }

private:
    QColor m_color{255, 255, 255};
};

// We need this custom shader to be able to set the PointSize in OpenGLES.
class ScopePointShader final : public QSGMaterialShader
{
public:
    inline static QSGMaterialType type;

    auto vertexShader() const -> char const* override
    {
        return "attribute highp vec4 vertex;       \n"
               "uniform highp mat4 matrix;         \n"
               "void main() {                      \n"
               "    gl_PointSize = 1.0;            \n"
               "    gl_Position = matrix * vertex; \n"
               "}";
    }

    auto fragmentShader() const -> char const* override
    {
        return "uniform lowp vec4 color;   \n"
               "void main() {              \n"
               "    gl_FragColor = color;  \n"
               "}";
    }

    auto attributeNames() const -> char const* const* override
    {
        static char const* const names[] = {"vertex", 0};
        return names;
    }

    void initialize() override
    {
        QSGMaterialShader::initialize();
        m_idMatrix = program()->uniformLocation("matrix");
        m_idColor = program()->uniformLocation("color");
    }

    void updateState(
            RenderState const& state,
            QSGMaterial* newEffect,
            QSGMaterial* oldEffect) override
    {
        Q_ASSERT(program()->isLinked());

        Q_ASSERT(
                oldEffect == nullptr || newEffect->type() == oldEffect->type());
        ScopePointMaterial* oldMaterial =
                static_cast<ScopePointMaterial*>(oldEffect);
        ScopePointMaterial* newMaterial =
                static_cast<ScopePointMaterial*>(newEffect);
        QColor const& c = newMaterial->color();
        if (oldMaterial == nullptr || c != oldMaterial->color() ||
            state.isOpacityDirty())
        {
            float opacity = state.opacity() * c.alphaF();
            QVector4D v(
                    c.redF() * opacity,
                    c.greenF() * opacity,
                    c.blueF() * opacity,
                    opacity);
            program()->setUniformValue(m_idColor, v);
        }
        if (state.isMatrixDirty())
            program()->setUniformValue(m_idMatrix, state.combinedMatrix());
    }

private:
    int m_idMatrix;
    int m_idColor;
};

auto
ScopePointMaterial::type() const noexcept -> QSGMaterialType*
{
    return &ScopePointShader::type;
}

auto
ScopePointMaterial::createShader() const -> QSGMaterialShader*
{
    return new ScopePointShader;
}

} // namespace

Scope::Scope(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents);
    connect(this, &Scope::heightChanged, [this]() {
        m_updateTransformMatrix = true;
        update();
    });
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

auto
Scope::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*) -> QSGNode*
{
    if (!m_lines)
        return nullptr;

    QSGTransformNode* node{};

    if (!oldNode)
    {
        node = new QSGTransformNode();
        node->appendChildNode(updateGeometry(nullptr));

        updateTransformMatrix(*node);
        m_updateTransformMatrix = false;
    }
    else
    {
        node = static_cast<QSGTransformNode*>(oldNode);
        if (m_updateTransformMatrix)
        {
            updateTransformMatrix(*node);
            m_updateTransformMatrix = false;
        }

        auto* geometryNode = node->firstChild();
        BOOST_ASSERT(geometryNode);
        BOOST_VERIFY(geometryNode == updateGeometry(geometryNode));
    }

    return node;
}

void
Scope::updateTransformMatrix(QSGTransformNode& node)
{
    QMatrix4x4 m;
    m.scale(1, size().height() / -2.f);
    m.translate(0, -1);
    node.setMatrix(m);

    node.markDirty(QSGNode::DirtyMatrix);
}

auto
Scope::updateGeometry(QSGNode* oldNode) -> QSGNode*
{
    QSGGeometryNode* node{};
    QSGGeometry* geometry{};
    QSGGeometryNode* pointsNode{};
    QSGGeometry* pointsGeometry{};

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
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 255, 255));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

        pointsNode = new QSGGeometryNode;
        pointsGeometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                geometry->vertexCount());
        pointsGeometry->setDrawingMode(QSGGeometry::DrawPoints);
        pointsNode->setGeometry(pointsGeometry);
        pointsNode->setFlag(QSGNode::OwnsGeometry);

        ScopePointMaterial* pointsMaterial = new ScopePointMaterial;
        pointsNode->setMaterial(pointsMaterial);
        pointsNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(pointsNode);
    }
    else
    {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();

        BOOST_ASSERT(node->childCount() == 1);
        pointsNode = static_cast<QSGGeometryNode*>(node->firstChild());
        pointsGeometry = pointsNode->geometry();

        if (static_cast<std::size_t>(geometry->vertexCount()) !=
            m_lines->get().size() * 2)
        {
            geometry->allocate(m_lines->get().size() * 2);
            setVertexX(
                    geometry->vertexDataAsPoint2D(),
                    geometry->vertexCount());

            pointsGeometry->allocate(geometry->vertexCount());
        }
    }

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    for (float const y : m_lines->get().ys())
    {
        vertices->y = y;
        ++vertices;
    }

    std::copy_n(
            geometry->vertexDataAsPoint2D(),
            geometry->vertexCount(),
            pointsGeometry->vertexDataAsPoint2D());

    node->markDirty(QSGNode::DirtyGeometry);
    pointsNode->markDirty(QSGNode::DirtyGeometry);

    return node;
}

} // namespace piejam::gui::item
