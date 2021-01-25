// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/Info.h>

#include <piejam/log/generic_log_sink.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/request_info_update.h>
#include <piejam/runtime/selectors.h>

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
                        auto qtMsg = QString::fromStdString(fmt::format(
                                "[{}] {}",
                                spdlog::level::to_string_view(msg.level),
                                msg.payload));
                        addLogMessage(qtMsg);
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
