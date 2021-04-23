// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModuleContent.h>
#include <piejam/gui/model/SpectrumData.h>
#include <piejam/gui/model/StereoChannel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxSpectrum : public FxModuleContent
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::SpectrumData* dataA READ dataA CONSTANT)
    Q_PROPERTY(piejam::gui::model::SpectrumData* dataB READ dataB CONSTANT)
    Q_PROPERTY(bool activeA READ activeA NOTIFY activeAChanged FINAL)
    Q_PROPERTY(bool activeB READ activeB NOTIFY activeBChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelA READ channelA NOTIFY
                       channelAChanged FINAL)
    Q_PROPERTY(piejam::gui::model::StereoChannel channelB READ channelB NOTIFY
                       channelBChanged FINAL)

public:
    auto type() const noexcept -> Type override final { return Type::Spectrum; }

    bool activeA() const noexcept { return m_activeA; }

    Q_INVOKABLE void changeActiveA(bool const active)
    {
        if (m_activeA != active)
        {
            m_activeA = active;
            emit activeAChanged();
            onActiveAChanged();
        }
    }

    auto channelA() const noexcept -> StereoChannel { return m_channelA; }

    Q_INVOKABLE void changeChannelA(piejam::gui::model::StereoChannel const x)
    {
        if (m_channelA != x)
        {
            m_channelA = x;
            emit channelAChanged();
            onChannelAChanged();
        }
    }

    auto dataA() noexcept -> SpectrumData* { return &m_spectrumDataA; }

    bool activeB() const noexcept { return m_activeB; }

    Q_INVOKABLE void changeActiveB(bool const active)
    {
        if (m_activeB != active)
        {
            m_activeB = active;
            emit activeBChanged();
            onActiveBChanged();
        }
    }

    auto channelB() const noexcept -> StereoChannel { return m_channelB; }

    Q_INVOKABLE void changeChannelB(piejam::gui::model::StereoChannel const x)
    {
        if (m_channelB != x)
        {
            m_channelB = x;
            emit channelBChanged();
            onChannelBChanged();
        }
    }

    auto dataB() noexcept -> SpectrumData* { return &m_spectrumDataB; }

    Q_INVOKABLE virtual void requestUpdate() = 0;

signals:
    void activeAChanged();
    void activeBChanged();
    void channelAChanged();
    void channelBChanged();

protected:
    virtual void onActiveAChanged() {}
    virtual void onActiveBChanged() {}
    virtual void onChannelAChanged() {}
    virtual void onChannelBChanged() {}

private:
    bool m_activeA{true};
    StereoChannel m_channelA{StereoChannel::Left};
    SpectrumData m_spectrumDataA;
    bool m_activeB{false};
    StereoChannel m_channelB{StereoChannel::Right};
    SpectrumData m_spectrumDataB;
};

} // namespace piejam::gui::model
