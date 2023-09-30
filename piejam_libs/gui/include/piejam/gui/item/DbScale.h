// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QQuickItem>

#include <memory>

namespace piejam::gui::item
{

class DbScale : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::DbScaleData* scaleData READ scaleData WRITE
                       setScaleData NOTIFY scaleDataChanged FINAL)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)

    Q_PROPERTY(int tickOffset READ tickOffset WRITE setTickOffset NOTIFY
                       tickOffsetChanged FINAL)

    QML_ELEMENT

public:
    explicit DbScale(QQuickItem* parent = nullptr);
    ~DbScale() override;

    auto scaleData() const noexcept -> model::DbScaleData*;
    void setScaleData(model::DbScaleData*);

    auto color() const noexcept -> QColor const&
    {
        return m_color;
    }

    void setColor(QColor const&);

    auto tickOffset() const noexcept -> int
    {
        return m_tickOffset;
    }

    void setTickOffset(int);

    enum class Edge : int
    {
        Left,
        Right
    };
    Q_ENUM(Edge)

    Q_PROPERTY(Edge edge READ edge WRITE setEdge NOTIFY edgeChanged FINAL)

    auto edge() const noexcept -> Edge
    {
        return m_edge;
    }

    void setEdge(Edge);

    auto updatePaintNode(QSGNode*, UpdatePaintNodeData*) -> QSGNode* override;

signals:
    void scaleDataChanged();
    void colorChanged();
    void tickOffsetChanged();
    void edgeChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QColor m_color{255, 255, 255};
    int m_tickOffset{};
    Edge m_edge{Edge::Left};
};

} // namespace piejam::gui::item
