// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/audio/types.h>
#include <piejam/runtime/device_io_fwd.h>
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
    Q_PROPERTY(double volume READ volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)
    Q_PROPERTY(double panBalance READ panBalance NOTIFY panBalanceChanged FINAL)
    Q_PROPERTY(bool record READ record NOTIFY recordChanged FINAL)
    Q_PROPERTY(bool mute READ mute NOTIFY muteChanged FINAL)
    Q_PROPERTY(bool solo READ solo NOTIFY soloChanged FINAL)
    Q_PROPERTY(
            bool mutedBySolo READ mutedBySolo NOTIFY mutedBySoloChanged FINAL)

    Q_PROPERTY(piejam::gui::model::MidiAssignable* volumeMidi READ volumeMidi
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::MidiAssignable* panMidi READ panMidi CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::MidiAssignable* muteMidi READ muteMidi
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::MidiAssignable* soloMidi READ soloMidi
                       CONSTANT FINAL)

public:
    MixerChannelPerform(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelPerform();

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

    auto volume() const noexcept -> double
    {
        return m_volume;
    }

    void setVolume(double const x)
    {
        if (m_volume != x)
        {
            m_volume = x;
            emit volumeChanged();
        }
    }

    Q_INVOKABLE void changeVolume(double);

    auto levelLeft() const noexcept -> double
    {
        return m_levelLeft;
    }

    auto levelRight() const noexcept -> double
    {
        return m_levelRight;
    }

    void setLevel(double left, double right)
    {
        if (m_levelLeft != left)
        {
            m_levelLeft = left;
            emit levelLeftChanged();
        }

        if (m_levelRight != right)
        {
            m_levelRight = right;
            emit levelRightChanged();
        }
    }

    auto panBalance() const noexcept -> double
    {
        return m_panBalance;
    }

    void setPanBalance(double const x)
    {
        if (m_panBalance != x)
        {
            m_panBalance = x;
            emit panBalanceChanged();
        }
    }

    Q_INVOKABLE void changePanBalance(double);

    auto record() const noexcept -> bool
    {
        return m_record;
    }

    void setRecord(bool x)
    {
        if (m_record != x)
        {
            m_record = x;
            emit recordChanged();
        }
    }

    Q_INVOKABLE void changeRecord(bool);

    auto mute() const noexcept -> bool
    {
        return m_mute;
    }

    void setMute(bool const x)
    {
        if (m_mute != x)
        {
            m_mute = x;
            emit muteChanged();
        }
    }

    Q_INVOKABLE void changeMute(bool);

    auto solo() const noexcept -> bool
    {
        return m_solo;
    }

    void setSolo(bool const x)
    {
        if (m_solo != x)
        {
            m_solo = x;
            emit soloChanged();
        }
    }

    Q_INVOKABLE void changeSolo(bool);

    auto mutedBySolo() const noexcept -> bool
    {
        return m_mutedBySolo;
    }

    void setMutedBySolo(bool const x)
    {
        if (m_mutedBySolo != x)
        {
            m_mutedBySolo = x;
            emit mutedBySoloChanged();
        }
    }

    auto volumeMidi() const noexcept -> MidiAssignable*;
    auto panMidi() const noexcept -> MidiAssignable*;
    auto muteMidi() const noexcept -> MidiAssignable*;
    auto soloMidi() const noexcept -> MidiAssignable*;

signals:

    void nameChanged();
    void monoChanged();
    void volumeChanged();
    void levelLeftChanged();
    void levelRightChanged();
    void panBalanceChanged();
    void recordChanged();
    void muteChanged();
    void soloChanged();
    void mutedBySoloChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    BusType m_busType{BusType::Mono};
    double m_volume{1.};
    double m_levelLeft{};
    double m_levelRight{};
    double m_panBalance{};
    bool m_record{};
    bool m_mute{};
    bool m_solo{};
    bool m_mutedBySolo{};
};

} // namespace piejam::gui::model
