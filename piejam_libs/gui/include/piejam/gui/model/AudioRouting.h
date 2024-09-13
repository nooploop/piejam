// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class AudioRouting final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString defaultName READ defaultName CONSTANT FINAL)
    Q_PROPERTY(bool defaultIsValid READ defaultIsValid NOTIFY
                       defaultIsValidChanged FINAL)
    Q_PROPERTY(SelectedState selectedState READ selectedState NOTIFY
                       selectedStateChanged FINAL)
    Q_PROPERTY(QString selected READ selected NOTIFY selectedChanged FINAL)
    Q_PROPERTY(QStringList devices READ devices NOTIFY devicesChanged FINAL)
    Q_PROPERTY(QStringList channels READ channels NOTIFY channelsChanged FINAL)

public:
    AudioRouting(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id,
            runtime::mixer::io_socket);
    ~AudioRouting() override;

    auto defaultName() const noexcept -> QString const&
    {
        return m_defaultName;
    }

    auto defaultIsValid() const noexcept -> bool
    {
        return m_defaultIsValid;
    }

    void setDefaultIsValid(bool x)
    {
        if (m_defaultIsValid != x)
        {
            m_defaultIsValid = x;
            emit defaultIsValidChanged();
        }
    }

    enum class SelectedState
    {
        Invalid,
        Valid,
        NotMixed
    };

    Q_ENUM(SelectedState)

    auto selectedState() const noexcept -> SelectedState
    {
        return m_selectedState;
    }

    void setSelectedState(SelectedState x)
    {
        if (m_selectedState != x)
        {
            m_selectedState = x;
            emit selectedStateChanged();
        }
    }

    auto selected() const noexcept -> QString const&
    {
        return m_selected;
    }

    void setSelected(QString const& x)
    {
        if (m_selected != x)
        {
            m_selected = x;
            emit selectedChanged();
        }
    }

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

    Q_INVOKABLE void changeToDefault();
    Q_INVOKABLE void changeToDevice(unsigned index);
    Q_INVOKABLE void changeToChannel(unsigned index);

signals:
    void defaultIsValidChanged();
    void selectedStateChanged();
    void selectedChanged();
    void devicesChanged();
    void channelsChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_defaultName;
    bool m_defaultIsValid{};
    SelectedState m_selectedState{};
    QString m_selected;
    QStringList m_devices;
    QStringList m_channels;
};

} // namespace piejam::gui::model
