// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
