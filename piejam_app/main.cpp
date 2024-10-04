// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/engine/processor.h>
#include <piejam/audio/sound_card_manager.h>
#include <piejam/fx_modules/init.h>
#include <piejam/gui/ModelManager.h>
#include <piejam/gui/init.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/qt_log.h>
#include <piejam/ladspa/instance_manager_processor_factory.h>
#include <piejam/ladspa/plugin.h>
#include <piejam/midi/device_manager.h>
#include <piejam/midi/device_update.h>
#include <piejam/midi/input_event_handler.h>
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

#include <boost/core/demangle.hpp>

#include <filesystem>

namespace
{

auto
config_file_path(piejam::runtime::locations const& locs)
{
    BOOST_ASSERT(!locs.config_dir.empty());

    if (!std::filesystem::exists(locs.config_dir))
    {
        std::filesystem::create_directories(locs.config_dir);
    }

    return locs.config_dir / "piejam.config";
}

constexpr int realtime_priority = 96;

struct QtThreadDelegator
{
    template <std::invocable F>
    void operator()(F&& f)
    {
        QMetaObject::invokeMethod(app, std::forward<F>(f));
    }

    QGuiApplication* app{};
};

} // namespace

namespace piejam::runtime::ui
{

auto
as_thunk_action(action const& a) -> thunk_action<runtime::state> const*
{
    return dynamic_cast<thunk_action<runtime::state> const*>(&a);
}

} // namespace piejam::runtime::ui

auto
main(int argc, char* argv[]) -> int
{
    using namespace piejam;

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    // run app on the second cpu, first one is for the system
    this_thread::set_affinity(1);

    piejam::gui::init();
    piejam::fx_modules::init();

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

    auto audio_device_manager = audio::make_sound_card_manager();
    auto midi_device_manager = midi::make_device_manager();
    ladspa::instance_manager_processor_factory ladspa_manager;

    using middleware_factory =
            redux::middleware_factory<runtime::state, runtime::action>;

    runtime::store store(
            [](runtime::state& st, runtime::action const& a) -> void {
                if (auto const* const ra =
                            dynamic_cast<runtime::reducible_action const*>(&a);
                    ra)
                {
                    ++st.reduce_count;
                    ra->reduce(st);
                }
                else
                {
                    auto const action_name =
                            boost::core::demangle(typeid(a).name());
                    spdlog::warn(
                            "non-reducible action detected: {}",
                            action_name);
                }
            },
            runtime::make_initial_state());

    store.apply_middleware(
            middleware_factory::make<runtime::persistence_middleware>());

    store.apply_middleware(
            middleware_factory::make<runtime::recorder_middleware>(
                    locs.rec_dir));

    store.apply_middleware(
            middleware_factory::make<runtime::audio_engine_middleware>(
                    thread::configuration{
                            .affinity = 2,
                            .realtime_priority = realtime_priority,
                            .name = "audio_main"},
                    std::array{
                            thread::configuration{
                                    .affinity = 3,
                                    .realtime_priority = realtime_priority,
                                    .name = "audio_worker_0"},
                            thread::configuration{
                                    .affinity = 0,
                                    .realtime_priority = realtime_priority,
                                    .name = "audio_worker_1"},
                            thread::configuration{
                                    .affinity = 1,
                                    .realtime_priority = realtime_priority,
                                    .name = "audio_worker_2"}},
                    *audio_device_manager,
                    ladspa_manager,
                    runtime::make_midi_input_controller(*midi_device_manager)));

    store.apply_middleware(
            middleware_factory::make<runtime::midi_control_middleware>(
                    [&midi_device_manager]() {
                        return midi_device_manager->update_devices();
                    }));

    store.apply_middleware(
            middleware_factory::make<runtime::ladspa_fx_middleware>(
                    ladspa_manager));

    store.apply_middleware(middleware_factory::make<redux::thunk_middleware>());

    store.apply_middleware(middleware_factory::make<
                           redux::queueing_middleware<runtime::action>>());

    store.apply_middleware(
            middleware_factory::make<
                    redux::thread_delegate_middleware<QtThreadDelegator>>(
                    std::this_thread::get_id(),
                    QtThreadDelegator{&app}));

    runtime::subscriber state_change_subscriber(
            [&store]() -> runtime::state const& { return store.state(); });

    store.subscribe([&state_change_subscriber](auto const& state) {
        state_change_subscriber.notify(state);
    });

    store.dispatch(runtime::actions::scan_ladspa_fx_plugins("/usr/lib/ladspa"));

    gui::ModelManager modelManager(store, state_change_subscriber);

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/");
    engine.rootContext()->setContextProperty("g_modelManager", &modelManager);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

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

        modelManager.info()->setCpuTemp(system::cpu_temp());

        avg_cpu_load.update();
        auto cpu_load_per_core = avg_cpu_load.per_core();
        modelManager.info()->setCpuLoad(QList<float>(
                cpu_load_per_core.begin(),
                cpu_load_per_core.end()));

        modelManager.info()->setDiskUsage(static_cast<int>(
                std::round(system::disk_usage(locs.home_dir) * 100)));
    });
    timer->start(std::chrono::seconds(1));

    auto const app_exec_result = app.exec();

    store.dispatch(runtime::actions::save_app_config(config_file_path(locs)));
    store.dispatch(runtime::actions::save_session(session_file));

    return app_exec_result;
}
