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

#include <piejam/log/generic_log_sink.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/selectors.h>
#include <piejam/runtime/state.h>

#include <spdlog/spdlog.h>

namespace piejam::app::gui::model
{

Info::Info(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
    spdlog::default_logger()->sinks().push_back(
            std::make_shared<log::generic_log_sink_mt>(
                    [this](spdlog::details::log_msg const& msg) {
                        addLogMessage(QString::fromStdString(fmt::format(
                                "[{}] {}",
                                spdlog::level::to_string_view(msg.level),
                                msg.payload)));
                    },
                    []() {}));
}

void
Info::onSubscribe()
{
    observe(runtime::selectors::select_xruns, [this](std::size_t const xruns) {
        setXRuns(static_cast<unsigned>(xruns));
    });

    observe(runtime::selectors::select_cpu_load,
            [this](float const cpu_load) { setAudioLoad(cpu_load); });
}

void
Info::requestUpdate()
{
    dispatch(runtime::actions::request_info_update{});
}

} // namespace piejam::app::gui::model
