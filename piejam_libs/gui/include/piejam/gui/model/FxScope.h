// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/ScopeLinesObject.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/fwd.h>

#include <QObject>

namespace piejam::gui::model
{

class FxScope : public FxModuleContent
{
    Q_OBJECT

    Q_PROPERTY(int samplesPerPoint READ samplesPerPoint WRITE setSamplesPerPoint
                       NOTIFY samplesPerPointChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* dataA READ dataA CONSTANT)
    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* dataB READ dataB CONSTANT)
    Q_PROPERTY(bool activeA READ activeA NOTIFY activeAChanged FINAL)
    Q_PROPERTY(bool activeB READ activeB NOTIFY activeBChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelA READ channelA NOTIFY
                       channelAChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelB READ channelB NOTIFY
                       channelBChanged FINAL)

public:
    auto type() const noexcept -> Type override final { return Type::Scope; }

    auto samplesPerPoint() const noexcept -> int { return m_samplesPerPoint; }
    void setSamplesPerPoint(int const x)
    {
        if (m_samplesPerPoint != x)
        {
            m_samplesPerPoint = x;
            emit samplesPerPointChanged();
        }
    }

    auto viewSize() const noexcept -> int { return m_viewSize; }
    void setViewSize(int const x)
    {
        if (m_viewSize != x)
        {
            m_viewSize = x;
            emit viewSizeChanged();

            clear();
        }
    }

    bool activeA() const noexcept { return m_activeA; }

    Q_INVOKABLE void changeActiveA(bool const active)
    {
        if (m_activeA != active)
        {
            m_activeA = active;
            emit activeAChanged();

            clear();
        }
    }

    auto channelA() const noexcept -> StereoChannel { return m_channelA; }

    Q_INVOKABLE void changeChannelA(piejam::gui::model::StereoChannel const x)
    {
        if (m_channelA != x)
        {
            m_channelA = x;
            emit channelAChanged();
        }
    }

    auto dataA() noexcept -> ScopeLinesObject* { return &m_linesA; }

    bool activeB() const noexcept { return m_activeB; }

    Q_INVOKABLE void changeActiveB(bool const active)
    {
        if (m_activeB != active)
        {
            m_activeB = active;
            emit activeBChanged();

            clear();
        }
    }

    auto channelB() const noexcept -> StereoChannel { return m_channelB; }

    Q_INVOKABLE void changeChannelB(piejam::gui::model::StereoChannel const x)
    {
        if (m_channelB != x)
        {
            m_channelB = x;
            emit channelBChanged();
        }
    }

    auto dataB() noexcept -> ScopeLinesObject* { return &m_linesB; }

    Q_INVOKABLE virtual void requestUpdate() = 0;

    Q_INVOKABLE void clear()
    {
        m_linesA.get().clear();

        if (m_activeA)
            m_linesA.get().resize(m_viewSize);

        m_linesA.update();

        m_linesB.get().clear();

        if (m_activeB)
            m_linesB.get().resize(m_viewSize);

        m_linesB.update();
    }

signals:
    void samplesPerPointChanged();
    void viewSizeChanged();
    void activeAChanged();
    void activeBChanged();
    void channelAChanged();
    void channelBChanged();

private:
    int m_samplesPerPoint{1};
    int m_viewSize{};
    bool m_activeA{true};
    StereoChannel m_channelA{StereoChannel::Left};
    ScopeLinesObject m_linesA;
    bool m_activeB{false};
    StereoChannel m_channelB{StereoChannel::Right};
    ScopeLinesObject m_linesB;
};

} // namespace piejam::gui::model
