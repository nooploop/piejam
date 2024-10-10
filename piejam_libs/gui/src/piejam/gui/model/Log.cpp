// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Log.h>

#include <piejam/log/generic_log_sink.h>
#include <piejam/redux/subscriptions_manager.h>
#include <piejam/runtime/selectors.h>

#include <fmt/chrono.h>

#include <spdlog/spdlog.h>

namespace piejam::gui::model
{

Log::Log(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
    spdlog::default_logger()->sinks().push_back(
            std::make_shared<log::generic_log_sink_mt>(
                    [this](spdlog::details::log_msg const& msg) {
                        auto qtMsg = QString::fromStdString(fmt::format(
                                "[{}] [{:%H:%M:%S}] {}",
                                spdlog::level::to_string_view(msg.level),
                                msg.time,
                                msg.payload));
                        addLogMessage(qtMsg);
                    },
                    []() {}));
}

void
Log::onSubscribe()
{
}

} // namespace piejam::gui::model
