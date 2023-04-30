// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/ScopeLinesObject.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class FxScope final : public Subscribable<FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(int samplesPerLine READ samplesPerLine WRITE setSamplesPerLine
                       NOTIFY samplesPerLineChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* dataA READ dataA CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::ScopeLinesObject* dataB READ dataB CONSTANT
                       FINAL)
    Q_PROPERTY(bool activeA READ activeA NOTIFY activeAChanged FINAL)
    Q_PROPERTY(bool activeB READ activeB NOTIFY activeBChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelA READ channelA NOTIFY
                       channelAChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelB READ channelB NOTIFY
                       channelBChanged FINAL)

public:
    FxScope(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxScope() override;

    auto type() const noexcept -> Type override
    {
        return Type::Scope;
    }

    auto samplesPerLine() const noexcept -> int
    {
        return m_samplesPerLine;
    }

    void setSamplesPerLine(int x);

    auto viewSize() const noexcept -> int
    {
        return m_viewSize;
    }

    void setViewSize(int const x)
    {
        if (m_viewSize != x)
        {
            m_viewSize = x;
            emit viewSizeChanged();

            clear();
        }
    }

    auto busType() const noexcept -> BusType
    {
        return m_busType;
    }

    auto activeA() const noexcept -> bool
    {
        return m_activeA;
    }

    Q_INVOKABLE void changeActiveA(bool active);

    auto channelA() const noexcept -> StereoChannel
    {
        return m_channelA;
    }

    Q_INVOKABLE void changeChannelA(piejam::gui::model::StereoChannel);

    auto dataA() noexcept -> ScopeLinesObject*
    {
        return &m_linesA;
    }

    auto activeB() const noexcept -> bool
    {
        return m_activeB;
    }

    Q_INVOKABLE void changeActiveB(bool active);

    auto channelB() const noexcept -> StereoChannel
    {
        return m_channelB;
    }

    Q_INVOKABLE void changeChannelB(piejam::gui::model::StereoChannel);

    auto dataB() noexcept -> ScopeLinesObject*
    {
        return &m_linesB;
    }

    Q_INVOKABLE void clear()
    {
        m_linesA.get().clear();

        if (m_activeA)
        {
            m_linesA.get().resize(m_viewSize);
        }

        m_linesA.update();

        m_linesB.get().clear();

        if (m_activeB)
        {
            m_linesB.get().resize(m_viewSize);
        }

        m_linesB.update();
    }

signals:
    void samplesPerLineChanged();
    void viewSizeChanged();
    void activeAChanged();
    void activeBChanged();
    void channelAChanged();
    void channelBChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_samplesPerLine{1};
    int m_viewSize{};
    BusType m_busType;
    bool m_activeA{true};
    StereoChannel m_channelA{StereoChannel::Left};
    ScopeLinesObject m_linesA;
    bool m_activeB{false};
    StereoChannel m_channelB{StereoChannel::Right};
    ScopeLinesObject m_linesB;
};

} // namespace piejam::gui::model
