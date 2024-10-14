// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/item/Waveform.h>

#include <piejam/gui/model/WaveformSlot.h>

#include <QQuickWindow>
#include <QSGFlatColorMaterial>
#include <QSGMaterial>

#include <boost/polymorphic_cast.hpp>

namespace piejam::gui::item
{

namespace
{

class WaveformPointMaterial : public QSGMaterial
{
public:
    auto type() const noexcept -> QSGMaterialType* override;

    auto createShader() const -> QSGMaterialShader* override;

    void setColor(QColor const& color)
    {
        m_color = color;
        setFlag(Blending, m_color.alpha() != 0xff);
    }

    auto color() const noexcept -> QColor const&
    {
        return m_color;
    }

    auto compare(QSGMaterial const* const other) const noexcept -> int override
    {
        auto const* const material =
                boost::polymorphic_downcast<WaveformPointMaterial const*>(
                        other);
        return m_color.rgba() - material->color().rgba();
    }

private:
    QColor m_color{255, 255, 255};
};

// We need this custom shader to be able to set the PointSize in OpenGLES.
class WaveformPointShader final : public QSGMaterialShader
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
        auto* const oldMaterial =
                boost::polymorphic_downcast<WaveformPointMaterial*>(oldEffect);
        auto* const newMaterial =
                boost::polymorphic_downcast<WaveformPointMaterial*>(newEffect);
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
        {
            program()->setUniformValue(m_idMatrix, state.combinedMatrix());
        }
    }

private:
    int m_idMatrix;
    int m_idColor;
};

auto
WaveformPointMaterial::type() const noexcept -> QSGMaterialType*
{
    return &WaveformPointShader::type;
}

auto
WaveformPointMaterial::createShader() const -> QSGMaterialShader*
{
    return new WaveformPointShader;
}

auto
updateGeometry(
        QSGNode* const oldNode,
        model::WaveformSlot const* const waveformData,
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
            vertices[0].x = static_cast<float>(i);
            vertices[1].x = static_cast<float>(i);
        }
    };

    auto const numLines = waveformData ? waveformData->get().size() : 0;

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

        WaveformPointMaterial* pointsMaterial = new WaveformPointMaterial;
        pointsMaterial->setColor(color);
        pointsNode->setMaterial(pointsMaterial);
        pointsNode->setFlag(QSGNode::OwnsMaterial);

        node->appendChildNode(pointsNode);
    }
    else
    {
        node = boost::polymorphic_downcast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();

        BOOST_ASSERT(node->childCount() == 1);
        pointsNode = boost::polymorphic_downcast<QSGGeometryNode*>(
                node->firstChild());
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
        if (waveformData)
        {
            QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
            for (float const y : waveformData->get().ys())
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
        boost::polymorphic_downcast<QSGFlatColorMaterial*>(node->material())
                ->setColor(color);
        boost::polymorphic_downcast<WaveformPointMaterial*>(
                pointsNode->material())
                ->setColor(color);

        node->markDirty(QSGNode::DirtyMaterial);
        pointsNode->markDirty(QSGNode::DirtyMaterial);

        dirtyMaterial = false;
    }

    return node;
}

} // namespace

struct Waveform::Impl
{
    bool transformMatrixDirty{true};
    bool waveformDirty{true};
    bool colorDirty{true};

    QPointer<model::WaveformSlot> waveform{};

    QMetaObject::Connection waveformChangedConnection;

    QColor color{255, 0, 0};
};

Waveform::Waveform(QQuickItem* parent)
    : QQuickItem(parent)
    , m_impl{make_pimpl<Impl>()}
{
    setFlag(ItemHasContents);
    connect(this, &Waveform::heightChanged, [this]() {
        m_impl->transformMatrixDirty = true;
        update();
    });
}

auto
Waveform::waveform() const noexcept -> model::WaveformSlot*
{
    return m_impl->waveform;
}

void
Waveform::setWaveform(model::WaveformSlot* x)
{
    if (m_impl->waveform != x)
    {
        m_impl->waveform = x;
        emit waveformChanged();

        if (m_impl->waveform)
        {
            m_impl->waveformChangedConnection = QObject::connect(
                    m_impl->waveform,
                    &model::WaveformSlot::changed,
                    this,
                    [this]() {
                        m_impl->waveformDirty = true;
                        update();
                    });
        }
        else
        {
            QObject::disconnect(m_impl->waveformChangedConnection);
        }

        m_impl->waveformDirty = true;
        update();
    }
}

auto
Waveform::color() const noexcept -> QColor const&
{
    return m_impl->color;
}

void
Waveform::setColor(QColor const& c)
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
Waveform::updatePaintNode(QSGNode* const oldNode, UpdatePaintNodeData*)
        -> QSGNode*
{
    QSGTransformNode* node{};

    if (!oldNode)
    {
        node = new QSGTransformNode();
        node->appendChildNode(updateGeometry(
                nullptr,
                m_impl->waveform,
                m_impl->color,
                m_impl->waveformDirty,
                m_impl->colorDirty));

        updateTransformMatrix(*node);
    }
    else
    {
        node = boost::polymorphic_downcast<QSGTransformNode*>(oldNode);
        if (m_impl->transformMatrixDirty)
        {
            updateTransformMatrix(*node);
        }

        auto* geometryNode = node->firstChild();
        BOOST_ASSERT(geometryNode);
        BOOST_VERIFY(
                geometryNode == updateGeometry(
                                        geometryNode,
                                        m_impl->waveform,
                                        m_impl->color,
                                        m_impl->waveformDirty,
                                        m_impl->colorDirty));
    }

    return node;
}

void
Waveform::updateTransformMatrix(QSGTransformNode& node)
{
    QMatrix4x4 m;
    m.scale(1, size().height() / -2.f);
    m.translate(0, -1);
    node.setMatrix(m);

    node.markDirty(QSGNode::DirtyMatrix);

    m_impl->transformMatrixDirty = false;
}

} // namespace piejam::gui::item
