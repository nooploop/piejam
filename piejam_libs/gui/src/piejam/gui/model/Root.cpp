// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2026  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/Root.h>

#include <piejam/gui/model/AudioDeviceSettings.h>
#include <piejam/gui/model/AudioInputOutputSettings.h>
#include <piejam/gui/model/FxBrowser.h>
#include <piejam/gui/model/FxModuleView.h>
#include <piejam/gui/model/Info.h>
#include <piejam/gui/model/Log.h>
#include <piejam/gui/model/MidiInputSettings.h>
#include <piejam/gui/model/Mixer.h>
#include <piejam/gui/model/SessionSettings.h>
#include <piejam/gui/model/UISettings.h>

#include <piejam/runtime/actions/root_view_actions.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

Root::Root(
    runtime::state_access const& state_access,
    std::filesystem::path sessions_dir)
    : CompositeSubscribableModel{state_access}
    , m_audioDeviceSettings{&addModel<AudioDeviceSettings>()}
    , m_audioInputSettings{&addModel<AudioInputOutputSettings>(
          io_direction::input)}
    , m_audioOutputSettings{&addModel<AudioInputOutputSettings>(
          io_direction::output)}
    , m_midiInputSettings{&addModel<MidiInputSettings>()}
    , m_uiSettings{&addModel<UISettings>()}
    , m_sessionSettings{&addModel<SessionSettings>(std::move(sessions_dir))}
    , m_mixer{&addModel<Mixer>()}
    , m_info{&addModel<Info>()}
    , m_log{&addModel<Log>()}
    , m_fxBrowser{&addModel<FxBrowser>()}
    , m_fxModule{&addModel<FxModuleView>()}
{
}

void
Root::showMixer()
{
    switchRootViewMode(runtime::root_view_mode::mixer);
}

void
Root::showFxModule()
{
    switchRootViewMode(runtime::root_view_mode::fx_module);
}

void
Root::showInfo()
{
    switchRootViewMode(runtime::root_view_mode::info);
}

void
Root::showSettings()
{
    switchRootViewMode(runtime::root_view_mode::settings);
}

void
Root::showPower()
{
    switchRootViewMode(runtime::root_view_mode::power);
}

void
Root::onSubscribe()
{
    observe(
        runtime::selectors::select_root_view_mode,
        [this](runtime::root_view_mode mode) {
            setMode(static_cast<Mode>(mode));
        });

    observe(
        runtime::selectors::select_focused_fx_module,
        [&](runtime::fx::module_id const fx_mod_id) {
            setCanShowFxModule(fx_mod_id.valid());
        });

    observe(
        runtime::selectors::select_display_rotation,
        [&](std::size_t rotation) {
            setDisplayRotation(static_cast<int>(rotation));
        });

    observe(
        runtime::selectors::select_on_screen_keyboard_enabled,
        [&](bool enabled) { setOnScreenKeyboardEnabled(enabled); });
}

void
Root::switchRootViewMode(runtime::root_view_mode mode)
{
    runtime::actions::set_root_view_mode action;
    action.mode = mode;
    dispatch(action);
}

} // namespace piejam::gui::model
