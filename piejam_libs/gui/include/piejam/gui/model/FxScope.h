// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/WaveformDataObject.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <QObject>

#include <memory>

namespace piejam::gui::model
{

class FxScope final : public Subscribable<FxModuleContent>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(int samplesPerPixel READ samplesPerPixel WRITE setSamplesPerPixel
                       NOTIFY samplesPerPixelChanged FINAL)
    Q_PROPERTY(int viewSize READ viewSize WRITE setViewSize NOTIFY
                       viewSizeChanged FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataA READ
                       waveformDataA CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::WaveformDataObject* waveformDataB READ
                       waveformDataB CONSTANT FINAL)
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

    auto busType() const noexcept -> BusType;

    auto samplesPerPixel() const noexcept -> int
    {
        return m_samplesPerPixel;
    }

    void setSamplesPerPixel(int x);

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

    auto waveformDataA() noexcept -> WaveformDataObject*
    {
        return &m_waveformDataA;
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

    auto waveformDataB() noexcept -> WaveformDataObject*
    {
        return &m_waveformDataB;
    }

    Q_INVOKABLE void clear();

signals:
    void samplesPerPixelChanged();
    void viewSizeChanged();
    void activeAChanged();
    void activeBChanged();
    void channelAChanged();
    void channelBChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    int m_samplesPerPixel{1};
    int m_viewSize{};
    BusType m_busType;
    bool m_activeA{true};
    StereoChannel m_channelA{StereoChannel::Left};
    WaveformDataObject m_waveformDataA;
    bool m_activeB{false};
    StereoChannel m_channelB{StereoChannel::Right};
    WaveformDataObject m_waveformDataB;
};

} // namespace piejam::gui::model
