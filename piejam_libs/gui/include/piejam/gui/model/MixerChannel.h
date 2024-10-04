// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/runtime/mixer_fwd.h>

namespace piejam::gui::model
{

class MixerChannel : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::BusType busType READ busType CONSTANT FINAL)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)
    Q_PROPERTY(MaterialColor color READ color NOTIFY colorChanged FINAL)

public:
    MixerChannel(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannel() override;

    auto busType() const noexcept -> BusType
    {
        return m_busType;
    }

    auto name() const noexcept -> QString const&
    {
        return m_name;
    }

    auto color() const noexcept -> MaterialColor
    {
        return m_color;
    }

signals:
    void nameChanged();
    void colorChanged();

protected:
    auto channel_id() const noexcept -> runtime::mixer::channel_id
    {
        return m_channel_id;
    }

    void onSubscribe() override;

private:
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    void setColor(MaterialColor x)
    {
        if (m_color != x)
        {
            m_color = x;
            emit colorChanged();
        }
    }

    runtime::mixer::channel_id m_channel_id;
    BusType m_busType{BusType::Mono};
    QString m_name;
    MaterialColor m_color;
};

} // namespace piejam::gui::model
