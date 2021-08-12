// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/runtime/device_io_fwd.h>

namespace piejam::gui::model
{

class BusConfig final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool mono READ mono NOTIFY monoChanged FINAL)
    Q_PROPERTY(int monoChannel READ monoChannel NOTIFY monoChannelChanged FINAL)
    Q_PROPERTY(int stereoLeftChannel READ stereoLeftChannel NOTIFY
                       stereoLeftChannelChanged FINAL)
    Q_PROPERTY(int stereoRightChannel READ stereoRightChannel NOTIFY
                       stereoRightChannelChanged FINAL)

public:
    BusConfig(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::device_io::bus_id);

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    Q_INVOKABLE void changeName(QString const&);

    auto mono() const noexcept -> bool { return m_mono; }
    void setMono(bool const x)
    {
        if (m_mono != x)
        {
            m_mono = x;
            emit monoChanged();
        }
    }

    auto monoChannel() const noexcept -> unsigned { return m_monoChannel; }
    void setMonoChannel(unsigned const x)
    {
        if (m_monoChannel != x)
        {
            m_monoChannel = x;
            emit monoChannelChanged();
        }
    }

    Q_INVOKABLE void changeMonoChannel(unsigned);

    auto stereoLeftChannel() const noexcept -> unsigned
    {
        return m_stereoLeftChannel;
    }

    void setStereoLeftChannel(unsigned const x)
    {
        if (m_stereoLeftChannel != x)
        {
            m_stereoLeftChannel = x;
            emit stereoLeftChannelChanged();
        }
    }

    Q_INVOKABLE void changeStereoLeftChannel(unsigned);

    auto stereoRightChannel() const noexcept -> unsigned
    {
        return m_stereoRightChannel;
    }

    void setStereoRightChannel(unsigned const x)
    {
        if (m_stereoRightChannel != x)
        {
            m_stereoRightChannel = x;
            emit stereoRightChannelChanged();
        }
    }

    Q_INVOKABLE void changeStereoRightChannel(unsigned);

    Q_INVOKABLE void deleteBus();

signals:

    void nameChanged();
    void monoChanged();
    void monoChannelChanged();
    void stereoLeftChannelChanged();
    void stereoRightChannelChanged();

private:
    void onSubscribe() override;

    runtime::device_io::bus_id m_bus_id;

    QString m_name;
    bool m_mono{true};
    unsigned m_monoChannel{};
    unsigned m_stereoLeftChannel{};
    unsigned m_stereoRightChannel{};
};

} // namespace piejam::gui::model
