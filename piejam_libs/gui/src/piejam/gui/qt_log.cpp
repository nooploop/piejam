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

#include <piejam/gui/qt_log.h>

#include <QString>
#include <QtGlobal>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

namespace piejam::gui::qt_log
{

template <class... Args>
static void
log_fn(QtMsgType type, char const* const pattern, Args const&... args)
{
    [type]() -> decltype(&spdlog::debug<Args...>) {
        switch (type)
        {
            case QtDebugMsg:
                return &spdlog::debug<Args...>;
            case QtInfoMsg:
                return &spdlog::info<Args...>;
            case QtWarningMsg:
                return &spdlog::warn<Args...>;
            case QtCriticalMsg:
                return &spdlog::critical<Args...>;
            case QtFatalMsg:
                return &spdlog::error<Args...>;
        }
        BOOST_ASSERT_MSG(false, "unhandled QtMsgType");
        return nullptr;
    }()(pattern, std::forward<Args const>(args)...);
}

void
log_message_handler(
        QtMsgType type,
        QMessageLogContext const& context,
        QString const& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    char const* const file = context.file ? context.file : "";
    char const* const function = context.function ? context.function : "";

    log_fn(type,
           "{} ({}:{}, {})",
           localMsg.constData(),
           file,
           context.line,
           function);
}

void
install_handler()
{
    qInstallMessageHandler(log_message_handler);
}

} // namespace piejam::gui::qt_log
