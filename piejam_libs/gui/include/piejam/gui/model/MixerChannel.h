// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
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

    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BusType, busType)
    M_PIEJAM_GUI_PROPERTY(QString, name, setName)
    M_PIEJAM_GUI_PROPERTY(MaterialColor, color, setColor)

public:
    MixerChannel(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannel() override;

protected:
    auto channel_id() const noexcept -> runtime::mixer::channel_id
    {
        return m_channel_id;
    }

    void onSubscribe() override;

private:
    runtime::mixer::channel_id m_channel_id;
    BusType m_busType{BusType::Mono};
};

} // namespace piejam::gui::model
