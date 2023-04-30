// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/BusType.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelEdit final : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)

    Q_PROPERTY(
            bool canMoveLeft READ canMoveLeft NOTIFY canMoveLeftChanged FINAL)

    Q_PROPERTY(bool canMoveRight READ canMoveRight NOTIFY canMoveRightChanged
                       FINAL)

    Q_PROPERTY(bool defaultInputIsValid READ defaultInputIsValid NOTIFY
                       defaultInputIsValidChanged FINAL)
    Q_PROPERTY(SelectedInputState selectedInputState READ selectedInputState
                       NOTIFY selectedInputStateChanged FINAL)
    Q_PROPERTY(QString selectedInput READ selectedInput NOTIFY
                       selectedInputChanged FINAL)
    Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY
                       inputDevicesChanged FINAL)
    Q_PROPERTY(QStringList inputChannels READ inputChannels NOTIFY
                       inputChannelsChanged FINAL)
    Q_PROPERTY(SelectedOutputState selectedOutputState READ selectedOutputState
                       NOTIFY selectedOutputStateChanged FINAL)
    Q_PROPERTY(QString selectedOutput READ selectedOutput NOTIFY
                       selectedOutputChanged FINAL)
    Q_PROPERTY(QStringList outputDevices READ outputDevices NOTIFY
                       outputDevicesChanged FINAL)
    Q_PROPERTY(QStringList outputChannels READ outputChannels NOTIFY
                       outputChannelsChanged FINAL)

public:
    MixerChannelEdit(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelEdit();

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

    Q_INVOKABLE void changeName(QString const&);

    auto canMoveLeft() const noexcept -> bool
    {
        return m_canMoveLeft;
    }

    void setCanMoveLeft(bool const x)
    {
        if (m_canMoveLeft != x)
        {
            m_canMoveLeft = x;
            emit canMoveLeftChanged();
        }
    }

    Q_INVOKABLE void moveLeft();

    auto canMoveRight() const noexcept -> bool
    {
        return m_canMoveRight;
    }

    void setCanMoveRight(bool const x)
    {
        if (m_canMoveRight != x)
        {
            m_canMoveRight = x;
            emit canMoveRightChanged();
        }
    }

    Q_INVOKABLE void moveRight();

    Q_INVOKABLE void deleteChannel();

    bool defaultInputIsValid() const noexcept
    {
        return m_defaultInputIsValid;
    }

    void setDefaultInputIsValid(bool x)
    {
        if (m_defaultInputIsValid != x)
        {
            m_defaultInputIsValid = x;
            emit defaultInputIsValidChanged();
        }
    }

    enum class SelectedInputState
    {
        Invalid,
        Valid
    };

    Q_ENUM(SelectedInputState)

    auto selectedInputState() const noexcept -> SelectedInputState
    {
        return m_selectedInputState;
    }

    void setSelectedInputState(SelectedInputState x)
    {
        if (m_selectedInputState != x)
        {
            m_selectedInputState = x;
            emit selectedInputStateChanged();
        }
    }

    auto selectedInput() const noexcept -> QString const&
    {
        return m_selectedInput;
    }

    void setSelectedInput(QString const& x)
    {
        if (m_selectedInput != x)
        {
            m_selectedInput = x;
            emit selectedInputChanged();
        }
    }

    auto inputDevices() const -> QStringList const&
    {
        return m_inputDevices;
    }

    void setInputDevices(QStringList const& x)
    {
        if (m_inputDevices != x)
        {
            m_inputDevices = x;
            emit inputDevicesChanged();
        }
    }

    auto inputChannels() const -> QStringList const&
    {
        return m_inputChannels;
    }

    void setInputChannels(QStringList const& x)
    {
        if (m_inputChannels != x)
        {
            m_inputChannels = x;
            emit inputChannelsChanged();
        }
    }

    Q_INVOKABLE void changeInputToDefault();
    Q_INVOKABLE void changeInputToDevice(unsigned index);
    Q_INVOKABLE void changeInputToChannel(unsigned index);

    enum class SelectedOutputState
    {
        Invalid,
        Valid,
        NotMixed
    };

    Q_ENUM(SelectedOutputState)

    auto selectedOutputState() const noexcept -> SelectedOutputState
    {
        return m_selectedOutputState;
    }

    void setSelectedOutputState(SelectedOutputState x)
    {
        if (m_selectedOutputState != x)
        {
            m_selectedOutputState = x;
            emit selectedOutputStateChanged();
        }
    }

    auto selectedOutput() const noexcept -> QString const&
    {
        return m_selectedOutput;
    }

    void setSelectedOutput(QString const& x)
    {
        if (m_selectedOutput != x)
        {
            m_selectedOutput = x;
            emit selectedOutputChanged();
        }
    }

    auto outputDevices() const -> QStringList const&
    {
        return m_outputDevices;
    }

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

    Q_INVOKABLE void changeOutputToNone();
    Q_INVOKABLE void changeOutputToDevice(unsigned index);
    Q_INVOKABLE void changeOutputToChannel(unsigned index);

signals:

    void nameChanged();
    void canMoveLeftChanged();
    void canMoveRightChanged();
    void defaultInputIsValidChanged();
    void selectedInputStateChanged();
    void selectedInputChanged();
    void inputDevicesChanged();
    void inputChannelsChanged();
    void selectedOutputStateChanged();
    void selectedOutputChanged();
    void outputDevicesChanged();
    void outputChannelsChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    BusType m_busType{BusType::Mono};
    bool m_canMoveLeft{};
    bool m_canMoveRight{};
    bool m_defaultInputIsValid{};
    SelectedInputState m_selectedInputState{};
    QString m_selectedInput;
    QStringList m_inputDevices;
    QStringList m_inputChannels;
    SelectedOutputState m_selectedOutputState{};
    QString m_selectedOutput;
    QStringList m_outputDevices;
    QStringList m_outputChannels;
};

} // namespace piejam::gui::model
