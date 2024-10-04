// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/MixerChannel.h>

#include <piejam/runtime/mixer_fwd.h>

#include <QStringList>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelEdit final : public MixerChannel
{
    Q_OBJECT

    Q_PROPERTY(
            bool canMoveLeft READ canMoveLeft NOTIFY canMoveLeftChanged FINAL)

    Q_PROPERTY(bool canMoveRight READ canMoveRight NOTIFY canMoveRightChanged
                       FINAL)

    Q_PROPERTY(piejam::gui::model::AudioRouting* in READ in CONSTANT FINAL)

    Q_PROPERTY(piejam::gui::model::AudioRouting* out READ out CONSTANT FINAL)

public:
    MixerChannelEdit(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelEdit() override;

    Q_INVOKABLE void changeName(QString const&);
    Q_INVOKABLE void changeColor(MaterialColor);

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

    auto in() const -> AudioRouting*;
    auto out() const -> AudioRouting*;

    Q_INVOKABLE void deleteChannel();

signals:
    void canMoveLeftChanged();
    void canMoveRightChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    bool m_canMoveLeft{};
    bool m_canMoveRight{};
};

} // namespace piejam::gui::model
