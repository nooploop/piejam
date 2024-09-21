// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/audio/types.h>
#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelPerform final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FloatParameter* volume READ volume CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::StereoLevelParameter* peakLevel READ
                       peakLevel CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::StereoLevelParameter* rmsLevel READ rmsLevel
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::FloatParameter* panBalance READ panBalance
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* record READ record CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* solo READ solo CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::BoolParameter* mute READ mute CONSTANT FINAL)
    Q_PROPERTY(
            bool mutedBySolo READ mutedBySolo NOTIFY mutedBySoloChanged FINAL)

public:
    MixerChannelPerform(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelPerform() override;

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    auto busType() const noexcept -> BusType
    {
        return m_busType;
    }

    auto volume() const noexcept -> FloatParameter*;
    auto peakLevel() const noexcept -> StereoLevelParameter*;
    auto rmsLevel() const noexcept -> StereoLevelParameter*;
    auto panBalance() const noexcept -> FloatParameter*;
    auto record() const noexcept -> BoolParameter*;
    auto solo() const noexcept -> BoolParameter*;
    auto mute() const noexcept -> BoolParameter*;

    auto mutedBySolo() const noexcept -> bool
    {
        return m_mutedBySolo;
    }

signals:
    void nameChanged();
    void mutedBySoloChanged();

private:
    void onSubscribe() override;

    void setMutedBySolo(bool const x)
    {
        if (m_mutedBySolo != x)
        {
            m_mutedBySolo = x;
            emit mutedBySoloChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    BusType m_busType{BusType::Mono};
    bool m_mutedBySolo{};
};

} // namespace piejam::gui::model
