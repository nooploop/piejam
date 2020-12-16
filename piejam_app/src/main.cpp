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

#include <piejam/app/gui/model/AudioDeviceSettings.h>
#include <piejam/app/gui/model/AudioInputOutputSettings.h>
#include <piejam/app/gui/model/FxBrowser.h>
#include <piejam/app/gui/model/FxChain.h>
#include <piejam/app/gui/model/Info.h>
#include <piejam/app/gui/model/Mixer.h>
#include <piejam/audio/alsa/get_pcm_io_descriptors.h>
#include <piejam/audio/alsa/get_set_hw_params.h>
#include <piejam/gui/model/BusConfig.h>
#include <piejam/gui/model/BusName.h>
#include <piejam/gui/model/FxModule.h>
#include <piejam/gui/model/FxParameter.h>
#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/qt_log.h>
#include <piejam/log/generic_log_sink.h>
#include <piejam/redux/queueing_middleware.h>
#include <piejam/redux/store.h>
#include <piejam/redux/thread_delegate_middleware.h>
#include <piejam/redux/thunk_middleware.h>
#include <piejam/reselect/subscriber.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/refresh_devices.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/actions/scan_ladspa_fx_plugins.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/audio_state.h>
#include <piejam/runtime/locations.h>
#include <piejam/runtime/open_alsa_device.h>
#include <piejam/runtime/persistence_middleware.h>
#include <piejam/runtime/store.h>
#include <piejam/runtime/subscriber.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/thunk_action.h>
#include <piejam/thread/affinity.h>

#include <QQuickStyle>
#include <QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>

static auto
config_file_path(piejam::runtime::locations const& locs)
{
    BOOST_ASSERT(!locs.config_dir.empty());

    if (!std::filesystem::exists(locs.config_dir))
        std::filesystem::create_directories(locs.config_dir);

    return locs.config_dir / "piejam.config";
}

auto
main(int argc, char* argv[]) -> int
{
    using namespace piejam;

    // run app on the second cpu, first one is for the system
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

    runtime::locations locs;
    locs.config_dir = QStandardPaths::writableLocation(
                              QStandardPaths::StandardLocation::ConfigLocation)
                              .toStdString();
    locs.home_dir = QStandardPaths::writableLocation(
                            QStandardPaths::StandardLocation::HomeLocation)
                            .toStdString();

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    Q_INIT_RESOURCE(piejam_gui_resources);
    Q_INIT_RESOURCE(piejam_app_resources);

    runtime::store store(
            [](auto const& st, auto const& a) { return a.reduce(st); },
            {});

    store.apply_middleware([](auto&& get_state, auto&& dispatch, auto&& next) {
        return redux::make_middleware<piejam::runtime::persistence_middleware>(
                std::forward<decltype(get_state)>(get_state),
                std::forward<decltype(dispatch)>(dispatch),
                std::forward<decltype(next)>(next));
    });

    store.apply_middleware([](auto&& get_state,
                              auto&& /*dispatch*/,
                              auto&& next) {
        thread::configuration const audio_thread_config{2, 96};
        std::array const worker_thread_configs{thread::configuration{3, 96}};
        return redux::make_middleware<runtime::audio_engine_middleware>(
                audio_thread_config,
                worker_thread_configs,
                &audio::alsa::get_pcm_io_descriptors,
                &audio::alsa::get_hw_params,
                &runtime::open_alsa_device,
                std::forward<decltype(get_state)>(get_state),
                std::forward<decltype(next)>(next));
    });

    store.apply_middleware(redux::make_thunk_middleware<
                           runtime::audio_state,
                           runtime::action>{});

    store.apply_middleware(redux::make_queueing_middleware);

    store.apply_middleware(redux::make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [app = &app](auto&& f) {
                QMetaObject::invokeMethod(app, std::forward<decltype(f)>(f));
            }));

    runtime::subscriber state_change_subscriber(
            [&store]() -> piejam::runtime::state const& {
                return store.state();
            });

    store.subscribe([&state_change_subscriber](auto const& state) {
        state_change_subscriber.notify(state);
    });

    store.dispatch(runtime::actions::scan_ladspa_fx_plugins("/usr/lib/ladspa"));

    app::gui::model::AudioDeviceSettings audio_settings(
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
    app::gui::model::FxChain fx_chain(store, state_change_subscriber);
    app::gui::model::FxBrowser fx_browser(store, state_change_subscriber);

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
    engine.rootContext()->setContextProperty("g_fx_chain", &fx_chain);
    engine.rootContext()->setContextProperty("g_fx_browser", &fx_browser);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(
            &engine,
            &QQmlApplicationEngine::quit,
            &QGuiApplication::quit);

    auto session_file = locs.home_dir / "last.pjs";

    store.dispatch(runtime::actions::refresh_devices{});
    store.dispatch(runtime::actions::load_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::load_session(session_file));

    auto const app_exec_result = app.exec();

    store.dispatch(runtime::actions::save_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::save_session(session_file));

    return app_exec_result;
}
