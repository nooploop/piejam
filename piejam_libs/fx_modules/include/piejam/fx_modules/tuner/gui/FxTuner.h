// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>

#include <memory>

namespace piejam::fx_modules::tuner::gui
{

class FxTuner final : public piejam::gui::model::FxModule
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::EnumParameter* channel READ channel CONSTANT
                       FINAL)

    Q_PROPERTY(float detectedFrequency READ detectedFrequency NOTIFY
                       detectedFrequencyChanged FINAL)

    Q_PROPERTY(QString detectedPitch READ detectedPitch NOTIFY
                       detectedPitchChanged FINAL)

    Q_PROPERTY(int detectedCents READ detectedCents NOTIFY detectedCentsChanged
                       FINAL)

public:
    FxTuner(runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);
    ~FxTuner() override;

    auto type() const noexcept -> piejam::gui::model::FxModuleType override;

    auto channel() const noexcept -> piejam::gui::model::EnumParameter*;

    auto detectedFrequency() const noexcept -> float
    {
        return m_detectedFrequency;
    }

    auto detectedPitch() const noexcept -> QString const&
    {
        return m_detectedPitch;
    }

    auto detectedCents() const noexcept -> int
    {
        return m_detectedCents;
    }

signals:
    void detectedFrequencyChanged();
    void detectedPitchChanged();
    void detectedCentsChanged();

private:
    void onSubscribe() override;

    void onChannelChanged();

    void setDetectedFrequency(float x)
    {
        if (m_detectedFrequency != x)
        {
            m_detectedFrequency = x;
            emit detectedFrequencyChanged();
        }
    }

    void setDetectedPitch(QString const& x)
    {
        if (m_detectedPitch != x)
        {
            m_detectedPitch = x;
            emit detectedPitchChanged();
        }
    }

    void setDetectedCents(int x)
    {
        if (m_detectedCents != x)
        {
            m_detectedCents = x;
            emit detectedCentsChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    float m_detectedFrequency{0.f};
    QString m_detectedPitch{"--"};
    int m_detectedCents{};
};

} // namespace piejam::fx_modules::tuner::gui
