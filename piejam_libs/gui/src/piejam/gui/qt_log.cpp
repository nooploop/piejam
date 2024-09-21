// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/qt_log.h>

#include <QString>
#include <QtGlobal>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

namespace piejam::gui::qt_log
{

void
log_message_handler(
        QtMsgType type,
        QMessageLogContext const& context,
        QString const& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    auto const msgData = localMsg.constData();
    char const* const file = context.file ? context.file : "";
    int const line = context.line;
    char const* const function = context.function ? context.function : "";

    constexpr auto const pattern = "{} ({}:{}, {})";

    switch (type)
    {
        case QtDebugMsg:
            spdlog::debug(pattern, msgData, file, line, function);
            break;
        case QtInfoMsg:
            spdlog::info(pattern, msgData, file, line, function);
            break;
        case QtWarningMsg:
            spdlog::warn(pattern, msgData, file, line, function);
            break;
        case QtCriticalMsg:
            spdlog::critical(pattern, msgData, file, line, function);
            break;
        case QtFatalMsg:
            spdlog::error(pattern, msgData, file, line, function);
            break;
        default:
            BOOST_ASSERT_MSG(false, "unhandled QtMsgType");
            break;
    }
}

void
install_handler()
{
    qInstallMessageHandler(log_message_handler);
}

} // namespace piejam::gui::qt_log
