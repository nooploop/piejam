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

#include <piejam/app/gui/model/Info.h>

#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/selectors.h>

namespace piejam::app::gui::model
{

Info::Info(store_dispatch store_dispatch, subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

void
Info::subscribeStep(
        subscriber& state_change_subscriber,
        subscriptions_manager& subs,
        subscription_id subs_id)
{
    subs.observe(
            subs_id,
            state_change_subscriber,
            runtime::selectors::select_xruns,
            [this](std::size_t const xruns) {
                setXRuns(static_cast<unsigned>(xruns));
            });

    subs.observe(
            subs_id,
            state_change_subscriber,
            runtime::selectors::select_cpu_load,
            [this](float const cpu_load) { setAudioLoad(cpu_load); });
}

void
Info::requestUpdate()
{
    dispatch(runtime::actions::request_info_update{});
}

} // namespace piejam::app::gui::model
