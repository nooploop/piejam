// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/fwd.h>

#include <QObject>
#include <QPointF>

#include <memory>

namespace piejam::gui::model
{

class FxScope : public QObject
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::AudioStreamListener* streamListener READ
                       streamListener CONSTANT FINAL)
    Q_PROPERTY(int samplesPerPoint READ samplesPerPoint WRITE setSamplesPerPoint
                       NOTIFY samplesPerPointChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(std::vector<QPointF> leftPoints READ leftPoints NOTIFY
                       leftPointsChanged FINAL)
    Q_PROPERTY(std::vector<QPointF> rightPoints READ rightPoints NOTIFY
                       rightPointsChanged FINAL)

public:
    FxScope(QObject* parent = nullptr);
    ~FxScope();

    auto streamListener() const noexcept -> AudioStreamListener*;

    auto samplesPerPoint() const noexcept -> int { return m_samplesPerPoint; }
    void setSamplesPerPoint(int);

    auto viewSize() const noexcept -> int { return m_viewSize; }
    void setViewSize(int);

    auto leftPoints() const noexcept -> std::vector<QPointF> const&
    {
        return m_leftPoints;
    }

    auto rightPoints() const noexcept -> std::vector<QPointF> const&
    {
        return m_rightPoints;
    }

    Q_INVOKABLE void clear();

signals:
    void samplesPerPointChanged();
    void viewSizeChanged();
    void leftPointsChanged();
    void rightPointsChanged();
    void syncedChanged();

private:
    std::unique_ptr<ScopePointsGenerator> m_streamListener;
    int m_samplesPerPoint{1};
    int m_viewSize{};
    std::vector<QPointF> m_leftPoints;
    std::vector<QPointF> m_rightPoints;
};

} // namespace piejam::gui::model
