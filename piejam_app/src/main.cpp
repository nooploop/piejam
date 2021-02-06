// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/app/gui/model/Factory.h>
#include <piejam/audio/alsa/get_pcm_io_descriptors.h>
#include <piejam/audio/alsa/get_set_hw_params.h>
#include <piejam/gui/qt_log.h>
#include <piejam/midi/device_manager.h>
#include <piejam/redux/batch_middleware.h>
#include <piejam/redux/queueing_middleware.h>
#include <piejam/redux/store.h>
#include <piejam/redux/thread_delegate_middleware.h>
#include <piejam/redux/thunk_middleware.h>
#include <piejam/reselect/subscriber.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/refresh_devices.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/actions/scan_ladspa_fx_plugins.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/locations.h>
#include <piejam/runtime/open_alsa_device.h>
#include <piejam/runtime/persistence_middleware.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/store.h>
#include <piejam/runtime/subscriber.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>
#include <piejam/thread/affinity.h>

#include <QQuickStyle>
#include <QStandardPaths>
#include <QTimer>
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

    auto midi_device_manager = midi::make_device_manager();

    store.apply_middleware([](auto&& get_state, auto&& dispatch, auto&& next) {
        thread::configuration const audio_thread_config{2, 96};
        std::array const worker_thread_configs{thread::configuration{3, 96}};
        return redux::make_middleware<runtime::audio_engine_middleware>(
                audio_thread_config,
                worker_thread_configs,
                &audio::alsa::get_pcm_io_descriptors,
                &audio::alsa::get_hw_params,
                &runtime::open_alsa_device,
                std::forward<decltype(get_state)>(get_state),
                std::forward<decltype(dispatch)>(dispatch),
                std::forward<decltype(next)>(next));
    });

    store.apply_middleware(
            redux::make_thunk_middleware<runtime::state, runtime::action>{});

    bool batching{};
    store.apply_middleware([&batching](
                                   auto&& /*get_state*/,
                                   auto&& /*dispatch*/,
                                   auto&& next) {
        return redux::make_middleware<redux::batch_middleware<runtime::action>>(
                batching,
                std::forward<decltype(next)>(next));
    });

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

    store.subscribe([&state_change_subscriber, &batching](auto const& state) {
        if (!batching)
            state_change_subscriber.notify(state);
    });

    store.dispatch(runtime::actions::scan_ladspa_fx_plugins("/usr/lib/ladspa"));

    app::gui::model::Factory model_factory(store, state_change_subscriber);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty(
            "g_modelFactory",
            static_cast<gui::model::Factory*>(&model_factory));

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject::connect(
            &engine,
            &QQmlApplicationEngine::quit,
            &QGuiApplication::quit);

    auto session_file = locs.home_dir / "last.pjs";

    store.dispatch(runtime::actions::refresh_devices{});
    store.dispatch(runtime::actions::refresh_midi_devices{});
    store.dispatch(runtime::actions::load_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::load_session(session_file));

    auto timer = new QTimer(&app);
    QObject::connect(timer, &QTimer::timeout, [&store]() {
        store.dispatch(runtime::actions::refresh_midi_devices{});
    });
    timer->start(std::chrono::seconds(1));

    auto const app_exec_result = app.exec();

    store.dispatch(runtime::actions::save_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::save_session(session_file));

    return app_exec_result;
}
