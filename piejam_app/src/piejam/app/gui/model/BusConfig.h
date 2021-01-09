// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/audio/types.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/runtime/mixer_fwd.h>

namespace piejam::app::gui::model
{

class BusConfig final : public Subscribable<piejam::gui::model::BusConfig>
{
public:
    BusConfig(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id);

    void changeName(QString const&) override;
    void changeMonoChannel(unsigned) override;
    void changeStereoLeftChannel(unsigned) override;
    void changeStereoRightChannel(unsigned) override;
    void deleteBus() override;

private:
    void onSubscribe() override;

    void changeChannel(audio::bus_channel, unsigned);

    runtime::mixer::bus_id m_bus_id;
};

} // namespace piejam::app::gui::model
