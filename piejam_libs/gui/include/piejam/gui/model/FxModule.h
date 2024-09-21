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

class FxModule : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(QString chainName READ chainName NOTIFY chainNameChanged FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool bypassed READ bypassed NOTIFY bypassedChanged FINAL)

    Q_PROPERTY(piejam::gui::model::FxModuleContent* content READ content NOTIFY
                       contentChanged FINAL)

public:
    FxModule(runtime::store_dispatch, runtime::subscriber&);
    ~FxModule();

    auto chainName() const noexcept -> QString const&
    {
        return m_chainName;
    }

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    bool bypassed() const noexcept
    {
        return m_bypassed;
    }

    auto content() noexcept -> FxModuleContent*;

    Q_INVOKABLE void toggleBypass();

signals:
    void chainNameChanged();
    void nameChanged();
    void bypassedChanged();
    void contentChanged();

private:
    void onSubscribe() override;

    void setChainName(QString const& x)
    {
        if (m_chainName != x)
        {
            m_chainName = x;
            emit chainNameChanged();
        }
    }

    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    void setBypassed(bool const x)
    {
        if (m_bypassed != x)
        {
            m_bypassed = x;
            emit bypassedChanged();
        }
    }

    struct Impl;
    std::unique_ptr<Impl> m_impl;

    QString m_chainName;
    QString m_name;
    bool m_bypassed{};
};

} // namespace piejam::gui::model
