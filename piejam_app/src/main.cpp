// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device_manager.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/gui/model/Factory.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/qt_log.h>
#include <piejam/ladspa/instance_manager_processor_factory.h>
#include <piejam/ladspa/plugin.h>
#include <piejam/midi/device_manager.h>
#include <piejam/midi/device_update.h>
#include <piejam/midi/input_event_handler.h>
#include <piejam/redux/batch_middleware.h>
#include <piejam/redux/queueing_middleware.h>
#include <piejam/redux/store.h>
#include <piejam/redux/thread_delegate_middleware.h>
#include <piejam/redux/thunk_middleware.h>
#include <piejam/reselect/subscriber.h>
#include <piejam/reselect/subscriptions_manager.h>
#include <piejam/runtime/actions/load_app_config.h>
#include <piejam/runtime/actions/load_session.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/actions/refresh_devices.h>
#include <piejam/runtime/actions/refresh_midi_devices.h>
#include <piejam/runtime/actions/save_app_config.h>
#include <piejam/runtime/actions/save_session.h>
#include <piejam/runtime/actions/scan_ladspa_fx_plugins.h>
#include <piejam/runtime/audio_engine_middleware.h>
#include <piejam/runtime/ladspa_fx_middleware.h>
#include <piejam/runtime/locations.h>
#include <piejam/runtime/midi_control_middleware.h>
#include <piejam/runtime/midi_input_controller.h>
#include <piejam/runtime/persistence_middleware.h>
#include <piejam/runtime/recorder_middleware.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/store.h>
#include <piejam/runtime/subscriber.h>
#include <piejam/runtime/ui/action.h>
#include <piejam/runtime/ui/action_tracker_middleware.h>
#include <piejam/runtime/ui/batch_action.h>
#include <piejam/runtime/ui/thunk_action.h>
#include <piejam/system/avg_cpu_load_tracker.h>
#include <piejam/system/cpu_temp.h>
#include <piejam/system/disk_usage.h>
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
#include <fstream>
#include <iostream>

static auto
config_file_path(piejam::runtime::locations const& locs)
{
    BOOST_ASSERT(!locs.config_dir.empty());

    if (!std::filesystem::exists(locs.config_dir))
        std::filesystem::create_directories(locs.config_dir);

    return locs.config_dir / "piejam.config";
}

constexpr int realtime_priority = 96;

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
    locs.rec_dir = locs.home_dir / "recordings";

    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Material");

    auto audio_device_manager = audio::make_device_manager();
    auto midi_device_manager = midi::make_device_manager();
    ladspa::instance_manager_processor_factory ladspa_manager;

    runtime::store store(
            [](auto const& st, auto const& a) { return a.reduce(st); },
            runtime::make_initial_state());

    store.apply_middleware([](auto&& get_state, auto&& dispatch, auto&& next) {
        return redux::make_middleware<runtime::persistence_middleware>(
                std::forward<decltype(get_state)>(get_state),
                std::forward<decltype(dispatch)>(dispatch),
                std::forward<decltype(next)>(next));
    });

    store.apply_middleware([rec_dir = locs.rec_dir](
                                   auto&& get_state,
                                   auto&& dispatch,
                                   auto&& next) {
        return redux::make_middleware<runtime::recorder_middleware>(
                runtime::middleware_functors(
                        std::forward<decltype(get_state)>(get_state),
                        std::forward<decltype(dispatch)>(dispatch),
                        std::forward<decltype(next)>(next)),
                rec_dir);
    });

    store.apply_middleware([audio_device_manager = audio_device_manager.get(),
                            midi_device_manager = midi_device_manager.get(),
                            &ladspa_manager](
                                   auto&& get_state,
                                   auto&& dispatch,
                                   auto&& next) {
        thread::configuration const audio_thread_config{
                2,
                realtime_priority,
                "audio_main"};
        std::array const worker_thread_configs{
                thread::configuration{3, realtime_priority, "audio_worker_0"},
                thread::configuration{0, realtime_priority, "audio_worker_1"},
                thread::configuration{1, realtime_priority, "audio_worker_2"},
        };
        return redux::make_middleware<runtime::audio_engine_middleware>(
                runtime::middleware_functors(
                        std::forward<decltype(get_state)>(get_state),
                        std::forward<decltype(dispatch)>(dispatch),
                        std::forward<decltype(next)>(next)),
                audio_thread_config,
                worker_thread_configs,
                *audio_device_manager,
                ladspa_manager,
                runtime::make_midi_input_controller(*midi_device_manager));
    });

    store.apply_middleware([midi_device_manager = midi_device_manager.get()](
                                   auto&& get_state,
                                   auto&& dispatch,
                                   auto&& next) {
        return redux::make_middleware<runtime::midi_control_middleware>(
                runtime::middleware_functors(
                        std::forward<decltype(get_state)>(get_state),
                        std::forward<decltype(dispatch)>(dispatch),
                        std::forward<decltype(next)>(next)),
                [midi_device_manager]() {
                    return midi_device_manager->update_devices();
                });
    });

    store.apply_middleware(
            [&ladspa_manager](auto&& get_state, auto&& dispatch, auto&& next) {
                return redux::make_middleware<runtime::ladspa_fx_middleware>(
                        runtime::middleware_functors(
                                std::forward<decltype(get_state)>(get_state),
                                std::forward<decltype(dispatch)>(dispatch),
                                std::forward<decltype(next)>(next)),
                        ladspa_manager);
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

    store.apply_middleware(
            [](auto&& /*get_state*/, auto&& /*dispatch*/, auto&& next) {
                return redux::make_middleware<
                        redux::queueing_middleware<runtime::action>>(
                        std::forward<decltype(next)>(next));
            });

    store.apply_middleware(redux::make_thread_delegate_middleware(
            std::this_thread::get_id(),
            [app = &app](auto&& f) {
                QMetaObject::invokeMethod(app, std::forward<decltype(f)>(f));
            }));

    runtime::subscriber state_change_subscriber(
            [&store]() -> runtime::state const& { return store.state(); });

    store.subscribe([&state_change_subscriber, &batching](auto const& state) {
        if (!batching)
            state_change_subscriber.notify(state);
    });

    store.dispatch(runtime::actions::scan_ladspa_fx_plugins("/usr/lib/ladspa"));

    gui::model::Factory modelFactory(store, state_change_subscriber);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty("g_modelFactory", &modelFactory);

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

    system::avg_cpu_load_tracker avg_cpu_load(4);

    auto timer = new QTimer(&app);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        store.dispatch(runtime::actions::refresh_midi_devices{});

        store.dispatch(runtime::actions::request_recorder_streams_update());

        modelFactory.info()->setCpuTemp(system::cpu_temp());

        avg_cpu_load.update();
        auto cpu_load_per_core = avg_cpu_load.per_core();
        modelFactory.info()->setCpuLoad(QList<float>(
                cpu_load_per_core.begin(),
                cpu_load_per_core.end()));

        modelFactory.info()->setDiskUsage(static_cast<int>(
                std::round(system::disk_usage(locs.home_dir) * 100)));
    });
    timer->start(std::chrono::seconds(1));

    auto const app_exec_result = app.exec();

    store.dispatch(runtime::actions::save_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::save_session(session_file));

    return app_exec_result;
}
