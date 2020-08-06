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

#include <piejam/app/config_access.h>
#include <piejam/app/gui/model/AudioInputSettings.h>
#include <piejam/app/gui/model/AudioOutputSettings.h>
#include <piejam/app/gui/model/AudioSettings.h>
#include <piejam/app/gui/model/Info.h>
#include <piejam/app/gui/model/Mixer.h>
#include <piejam/app/store.h>
#include <piejam/app/subscriber.h>
#include <piejam/audio/alsa/get_pcm_io_descriptors.h>
#include <piejam/audio/alsa/get_set_hw_params.h>
#include <piejam/gui/qt_log.h>
#include <piejam/log/generic_log_sink.h>
#include <piejam/redux/store.h>
#include <piejam/reselect/subscriber.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/device_actions.h>
#include <piejam/runtime/app_config.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/open_alsa_device.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/thread/affinity.h>

#include <QQuickStyle>
#include <QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>

auto
main(int argc, char* argv[]) -> int
{
    using namespace piejam;

    // run app on the second cpu
    this_thread::set_affinity(1);

    gui::qt_log::install_handler();
    spdlog::set_level(spdlog::level::level_enum::debug);

    auto log_file_directory =
            QStandardPaths::writableLocation(
                    QStandardPaths::StandardLocation::HomeLocation)
                    .toStdString();
    std::filesystem::create_directories(log_file_directory);
    spdlog::default_logger()->sinks().push_back(
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                    log_file_directory + "/piejam.log",
                    true));

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    Q_INIT_RESOURCE(piejam_gui_resources);
    Q_INIT_RESOURCE(piejam_app_resources);

    app::store store([](auto const& st, auto const& a) { return a(st); }, {});

    store.apply_middleware([](auto get_state, auto /*dispatch*/, auto next) {
        auto m = std::make_shared<runtime::audio_engine_middleware>(
                3, /* audio_thread_cpu_affinity */
                &audio::alsa::get_pcm_io_descriptors,
                &audio::alsa::get_hw_params,
                &runtime::open_alsa_device,
                std::move(get_state),
                std::move(next));
        return [m](auto const& a) { m->operator()(a); };
    });

    piejam::reselect::subscriber<runtime::audio_state> state_change_subscriber;

    store.subscribe([&state_change_subscriber](auto const& state) {
        state_change_subscriber.notify(state);
    });

    qmlRegisterInterface<gui::model::BusConfig>("BusConfig");
    qmlRegisterInterface<gui::model::MixerChannel>("MixerChannel");

    app::gui::model::AudioSettings audio_settings(
            store,
            state_change_subscriber);
    app::gui::model::AudioInputSettings audio_input_settings(
            store,
            state_change_subscriber);
    app::gui::model::AudioOutputSettings audio_output_settings(
            store,
            state_change_subscriber);
    app::gui::model::Mixer mixer(store, state_change_subscriber);
    app::gui::model::Info info_model(store, state_change_subscriber);

    spdlog::default_logger()->sinks().push_back(
            std::make_shared<core::generic_log_sink_mt>(
                    [&info_model](spdlog::details::log_msg const& msg) {
                        info_model.addLogMessage(
                                QString::fromStdString(fmt::format(
                                        "[{}] {}",
                                        static_cast<int>(msg.level),
                                        msg.payload)));
                    },
                    []() {}));

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty(
            "g_audioSettings",
            &audio_settings);
    engine.rootContext()->setContextProperty(
            "g_audioInputSettings",
            &audio_input_settings);
    engine.rootContext()->setContextProperty(
            "g_audioOutputSettings",
            &audio_output_settings);
    engine.rootContext()->setContextProperty("g_mixer", &mixer);
    engine.rootContext()->setContextProperty("g_info", &info_model);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(
            &engine,
            &QQmlApplicationEngine::quit,
            &QGuiApplication::quit);

    store.dispatch<runtime::actions::refresh_devices>();

    app::config_access::load(store);

    auto const app_exec_result = app.exec();

    app::config_access::save(store.state());

    return app_exec_result;
}
