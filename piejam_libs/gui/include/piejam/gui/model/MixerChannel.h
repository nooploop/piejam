// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QStringList>

namespace piejam::gui::model
{

class MixerChannel : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool mono READ mono NOTIFY monoChanged FINAL)
    Q_PROPERTY(double volume READ volume NOTIFY volumeChanged FINAL)
    Q_PROPERTY(double levelLeft READ levelLeft NOTIFY levelLeftChanged FINAL)
    Q_PROPERTY(double levelRight READ levelRight NOTIFY levelRightChanged FINAL)
    Q_PROPERTY(double panBalance READ panBalance NOTIFY panBalanceChanged FINAL)
    Q_PROPERTY(bool mute READ mute NOTIFY muteChanged FINAL)
    Q_PROPERTY(bool solo READ solo NOTIFY soloChanged FINAL)

    Q_PROPERTY(piejam::gui::model::MidiAssignable* volumeMidi READ volumeMidi
                       CONSTANT FINAL)
    Q_PROPERTY(piejam::gui::model::MidiAssignable* panMidi READ panMidi CONSTANT
                       FINAL)
    Q_PROPERTY(piejam::gui::model::MidiAssignable* muteMidi READ muteMidi
                       CONSTANT FINAL)

    Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY
                       inputDevicesChanged FINAL)
    Q_PROPERTY(QStringList inputChannels READ inputChannels NOTIFY
                       inputChannelsChanged FINAL)
    Q_PROPERTY(QStringList outputDevices READ outputDevices NOTIFY
                       outputDevicesChanged FINAL)
    Q_PROPERTY(QStringList outputChannels READ outputChannels NOTIFY
                       outputChannelsChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const&);
    Q_INVOKABLE virtual void changeName(QString const&) = 0;

    auto mono() const noexcept -> bool { return m_mono; }
    void setMono(bool x)
    {
        if (m_mono != x)
        {
            m_mono = x;
            emit monoChanged();
        }
    }

    auto volume() const noexcept -> double { return m_volume; }
    void setVolume(double);
    Q_INVOKABLE virtual void changeVolume(double) = 0;

    auto levelLeft() const noexcept -> double { return m_levelLeft; }
    auto levelRight() const noexcept -> double { return m_levelRight; }
    void setLevel(double left, double right);

    auto panBalance() const noexcept -> double { return m_panBalance; }
    void setPanBalance(double);
    Q_INVOKABLE virtual void changePanBalance(double) = 0;

    auto mute() const noexcept -> bool { return m_mute; }
    void setMute(bool);
    Q_INVOKABLE virtual void changeMute(bool) = 0;

    auto solo() const noexcept -> bool { return m_solo; }
    void setSolo(bool);

    Q_INVOKABLE virtual void focusFxChain() = 0;

    Q_INVOKABLE virtual void deleteChannel() = 0;

    virtual auto volumeMidi() const -> MidiAssignable* = 0;
    virtual auto panMidi() const -> MidiAssignable* = 0;
    virtual auto muteMidi() const -> MidiAssignable* = 0;

    auto inputDevices() const -> QStringList const& { return m_inputDevices; }
    void setInputDevices(QStringList const& x)
    {
        if (m_inputDevices != x)
        {
            m_inputDevices = x;
            emit inputDevicesChanged();
        }
    }

    auto inputChannels() const -> QStringList const& { return m_inputChannels; }
    void setInputChannels(QStringList const& x)
    {
        if (m_inputChannels != x)
        {
            m_inputChannels = x;
            emit inputChannelsChanged();
        }
    }

    auto outputDevices() const -> QStringList const& { return m_outputDevices; }
    void setOutputDevices(QStringList const& x)
    {
        if (m_outputDevices != x)
        {
            m_outputDevices = x;
            emit outputDevicesChanged();
        }
    }

    auto outputChannels() const -> QStringList const&
    {
        return m_outputChannels;
    }

    void setOutputChannels(QStringList const& x)
    {
        if (m_outputChannels != x)
        {
            m_outputChannels = x;
            emit outputChannelsChanged();
        }
    }

signals:

    void nameChanged();
    void monoChanged();
    void volumeChanged();
    void levelLeftChanged();
    void levelRightChanged();
    void panBalanceChanged();
    void muteChanged();
    void soloChanged();
    void inputDevicesChanged();
    void inputChannelsChanged();
    void outputDevicesChanged();
    void outputChannelsChanged();

private:
    QString m_name;
    bool m_mono{};
    double m_volume{1.};
    double m_levelLeft{};
    double m_levelRight{};
    double m_panBalance{};
    bool m_mute{};
    bool m_solo{};
    QStringList m_inputDevices;
    QStringList m_inputChannels;
    QStringList m_outputDevices;
    QStringList m_outputChannels;
};

} // namespace piejam::gui::model
