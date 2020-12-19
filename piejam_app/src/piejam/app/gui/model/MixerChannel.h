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
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

namespace piejam::app::gui::model
{

class MixerChannel final : public Subscribable<piejam::gui::model::MixerChannel>
{
public:
    MixerChannel(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::bus_id,
            runtime::float_parameter_id volume,
            runtime::float_parameter_id pan_balance,
            runtime::bool_parameter_id mute,
            runtime::stereo_level_parameter_id level);

    void changeVolume(double) override;
    void changePanBalance(double) override;
    void changeMute(bool) override;
    void focusFxChain() override;

private:
    void onSubscribe() override;

    runtime::mixer::bus_id m_bus_id;
    runtime::float_parameter_id m_volume;
    runtime::float_parameter_id m_pan_balance;
    runtime::bool_parameter_id m_mute;
    runtime::stereo_level_parameter_id m_level;
};

} // namespace piejam::app::gui::model
