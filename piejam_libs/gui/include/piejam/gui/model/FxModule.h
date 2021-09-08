// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/mixer_fwd.h>

#include <memory>

namespace piejam::gui::model
{

class FxModule : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool bypassed READ bypassed NOTIFY bypassedChanged FINAL)
    Q_PROPERTY(
            bool canMoveLeft READ canMoveLeft NOTIFY canMoveLeftChanged FINAL)
    Q_PROPERTY(bool canMoveRight READ canMoveRight NOTIFY canMoveRightChanged
                       FINAL)
    Q_PROPERTY(piejam::gui::model::FxModuleContent* content READ content
                       CONSTANT FINAL)

public:
    FxModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id fx_chain_id,
            runtime::fx::module_id);
    ~FxModule();

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    bool bypassed() const noexcept { return m_bypassed; }
    void setBypassed(bool const x)
    {
        if (m_bypassed != x)
        {
            m_bypassed = x;
            emit bypassedChanged();
        }
    }

    auto canMoveLeft() const noexcept -> bool { return m_canMoveLeft; }
    void setCanMoveLeft(bool x)
    {
        if (m_canMoveLeft != x)
        {
            m_canMoveLeft = x;
            emit canMoveLeftChanged();
        }
    }

    auto canMoveRight() const noexcept -> bool { return m_canMoveRight; }
    void setCanMoveRight(bool x)
    {
        if (m_canMoveRight != x)
        {
            m_canMoveRight = x;
            emit canMoveRightChanged();
        }
    }

    auto content() noexcept -> FxModuleContent*;

    Q_INVOKABLE void toggleBypass();
    Q_INVOKABLE void deleteModule();
    Q_INVOKABLE void moveLeft();
    Q_INVOKABLE void moveRight();

signals:
    void nameChanged();
    void bypassedChanged();
    void canMoveLeftChanged();
    void canMoveRightChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    bool m_bypassed{};
    bool m_canMoveLeft{};
    bool m_canMoveRight{};
};

} // namespace piejam::gui::model
