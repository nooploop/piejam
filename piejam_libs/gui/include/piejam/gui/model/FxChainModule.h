// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
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

class FxChainModule : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool focused READ focused NOTIFY focusedChanged FINAL)

public:
    FxChainModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id fx_chain_id,
            runtime::fx::module_id);
    ~FxChainModule();

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

    auto focused() const noexcept -> bool
    {
        return m_focused;
    }

    void setFocused(bool x)
    {
        if (m_focused != x)
        {
            m_focused = x;
            emit focusedChanged();
        }
    }

    Q_INVOKABLE void remove();
    Q_INVOKABLE void focus();
    Q_INVOKABLE void showFxModule();

signals:
    void nameChanged();
    void focusedChanged();

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_name;
    bool m_focused{};
};

} // namespace piejam::gui::model
