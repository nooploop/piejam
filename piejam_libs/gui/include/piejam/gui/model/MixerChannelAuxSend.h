// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelAuxSend final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(piejam::gui::model::AudioRoutingSelection* selected READ selected
                       CONSTANT FINAL)
    Q_PROPERTY(bool canToggle READ canToggle NOTIFY canToggleChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(piejam::gui::model::FloatParameter* volume READ volume NOTIFY
                       volumeChanged FINAL)
    Q_PROPERTY(QStringList devices READ devices NOTIFY devicesChanged FINAL)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged FINAL)

public:
    MixerChannelAuxSend(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelAuxSend() override;

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

    auto selected() const noexcept -> AudioRoutingSelection*;

    auto canToggle() const noexcept -> bool
    {
        return m_canToggle;
    }

    void setCanToggle(bool x)
    {
        if (m_canToggle != x)
        {
            m_canToggle = x;
            emit canToggleChanged();
        }
    }

    auto enabled() const noexcept -> bool
    {
        return m_enabled;
    }

    void setEnabled(bool x)
    {
        if (m_enabled != x)
        {
            m_enabled = x;
            emit enabledChanged();
        }
    }

    auto volume() const noexcept -> FloatParameter*;

    auto devices() const -> QStringList const&
    {
        return m_devices;
    }

    void setDevices(QStringList const& x)
    {
        if (m_devices != x)
        {
            m_devices = x;
            emit devicesChanged();
        }
    }

    auto channels() const -> QStringList const&
    {
        return m_channels;
    }

    void setChannels(QStringList const& x)
    {
        if (m_channels != x)
        {
            m_channels = x;
            emit channelsChanged();
        }
    }

    Q_INVOKABLE void toggleEnabled();
    Q_INVOKABLE void changeToDevice(unsigned index);
    Q_INVOKABLE void changeToChannel(unsigned index);

signals:
    void nameChanged();
    void canToggleChanged();
    void enabledChanged();
    void volumeChanged();
    void devicesChanged();
    void channelsChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    bool m_canToggle{};
    bool m_enabled{};
    QStringList m_devices;
    QStringList m_channels;
};

} // namespace piejam::gui::model
