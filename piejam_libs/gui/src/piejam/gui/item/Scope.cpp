// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Scope.h>

#include <piejam/gui/model/ScopeLinesObject.h>

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

struct Scope::Impl
{
    bool transformMatrixDirty{true};
    bool linesADirty{true};
    bool linesBDirty{true};
    bool colorADirty{true};
    bool colorBDirty{true};

    model::ScopeLinesObject* linesA{};
    model::ScopeLinesObject* linesB{};

    QMetaObject::Connection linesAChangedConnection;
    QMetaObject::Connection linesBChangedConnection;

    QColor colorA{255, 0, 0};
    QColor colorB{0, 0, 255};
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
Scope::linesA() const noexcept -> model::ScopeLinesObject*
{
    return m_impl->linesA;
}

void
Scope::setLinesA(model::ScopeLinesObject* x)
{
    if (m_impl->linesA != x)
    {
        m_impl->linesA = x;
        emit linesAChanged();

        if (m_impl->linesA)
        {
            m_impl->linesAChangedConnection = QObject::connect(
                    m_impl->linesA,
                    &model::ScopeLinesObject::changed,
                    this,
                    [this]() {
                        m_impl->linesADirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->linesAChangedConnection);
        }

        m_impl->linesADirty = true;
        update();
    }
}

auto
Scope::linesB() const noexcept -> model::ScopeLinesObject*
{
    return m_impl->linesB;
}

void
Scope::setLinesB(model::ScopeLinesObject* x)
{
    if (m_impl->linesB != x)
    {
        m_impl->linesB = x;
        emit linesBChanged();

        if (m_impl->linesB)
        {
            m_impl->linesBChangedConnection = QObject::connect(
                    m_impl->linesB,
                    &model::ScopeLinesObject::changed,
                    this,
                    [this]() {
                        m_impl->linesBDirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->linesBChangedConnection);
        }

        m_impl->linesBDirty = true;
        update();
    }
}

auto
Scope::colorA() const noexcept -> QColor const&
{
    return m_impl->colorA;
}

void
Scope::setColorA(QColor const& c)
{
    if (m_impl->colorA != c)
    {
        m_impl->colorA = c;
        emit colorAChanged();

        m_impl->colorADirty = true;
        update();
    }
}

auto
Scope::colorB() const noexcept -> QColor const&
{
    return m_impl->colorB;
}

void
Scope::setColorB(QColor const& c)
{
    if (m_impl->colorB != c)
    {
        m_impl->colorB = c;
        emit colorBChanged();

        m_impl->colorBDirty = true;
        update();
    }
}

auto
Scope::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*) -> QSGNode*
{
    QSGTransformNode* node{};

    if (!oldNode)
    {
        node = new QSGTransformNode();
        node->appendChildNode(updateGeometry(
                nullptr,
                m_impl->linesA,
                m_impl->colorA,
                m_impl->linesADirty,
                m_impl->colorADirty));
        node->appendChildNode(updateGeometry(
                nullptr,
                m_impl->linesB,
                m_impl->colorB,
                m_impl->linesBDirty,
                m_impl->colorBDirty));

        updateTransformMatrix(*node);
    }
    else
    {
        node = static_cast<QSGTransformNode*>(oldNode);
        if (m_impl->transformMatrixDirty)
        {
            updateTransformMatrix(*node);
        }

        auto* geometryNodeA = node->firstChild();
        BOOST_ASSERT(geometryNodeA);
        BOOST_VERIFY(
                geometryNodeA == updateGeometry(
                                         geometryNodeA,
                                         m_impl->linesA,
                                         m_impl->colorA,
                                         m_impl->linesADirty,
                                         m_impl->colorADirty));

        auto* geometryNodeB = geometryNodeA->nextSibling();
        BOOST_ASSERT(geometryNodeB);
        BOOST_VERIFY(
                geometryNodeB == updateGeometry(
                                         geometryNodeB,
                                         m_impl->linesB,
                                         m_impl->colorB,
                                         m_impl->linesBDirty,
                                         m_impl->colorBDirty));
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

    m_impl->transformMatrixDirty = false;
}

auto
Scope::updateGeometry(
        QSGNode* oldNode,
        model::ScopeLinesObject const* lines,
        QColor const& color,
        bool& dirtyGeometry,
        bool& dirtyMaterial) -> QSGNode*
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

    auto const numLines = lines ? lines->get().size() : 0;

    if (!oldNode)
    {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(
                QSGGeometry::defaultAttributes_Point2D(),
                numLines * 2);
        setVertexX(geometry->vertexDataAsPoint2D(), geometry->vertexCount());
        geometry->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(color);
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
        pointsMaterial->setColor(color);
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

        if (static_cast<std::size_t>(geometry->vertexCount()) != numLines * 2)
        {
            geometry->allocate(numLines * 2);
            setVertexX(
                    geometry->vertexDataAsPoint2D(),
                    geometry->vertexCount());

            pointsGeometry->allocate(geometry->vertexCount());
        }
    }

    if (dirtyGeometry)
    {
        if (lines)
        {
            QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
            for (float const y : lines->get().ys())
            {
                vertices->y = y;
                ++vertices;
            }

            std::copy_n(
                    geometry->vertexDataAsPoint2D(),
                    geometry->vertexCount(),
                    pointsGeometry->vertexDataAsPoint2D());
        }

        node->markDirty(QSGNode::DirtyGeometry);
        pointsNode->markDirty(QSGNode::DirtyGeometry);

        dirtyGeometry = false;
    }

    if (dirtyMaterial)
    {
        static_cast<QSGFlatColorMaterial*>(node->material())->setColor(color);
        static_cast<ScopePointMaterial*>(pointsNode->material())
                ->setColor(color);

        node->markDirty(QSGNode::DirtyMaterial);
        pointsNode->markDirty(QSGNode::DirtyMaterial);

        dirtyMaterial = false;
    }

    return node;
}

} // namespace piejam::gui::item
