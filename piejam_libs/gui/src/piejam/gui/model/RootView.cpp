// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/RootView.h>

#include <piejam/runtime/actions/root_view_actions.h>
#include <piejam/runtime/selectors.h>
#include <piejam/to_underlying.h>

namespace piejam::gui::model
{

RootView::RootView(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber)
    : Subscribable(store_dispatch, state_change_subscriber)
{
}

RootView::~RootView() = default;

void
RootView::showMixer()
{
    switchRootViewMode(runtime::root_view_mode::mixer);
}

void
RootView::showInfo()
{
    switchRootViewMode(runtime::root_view_mode::info);
}

void
RootView::showSettings()
{
    switchRootViewMode(runtime::root_view_mode::settings);
}

void
RootView::showPower()
{
    switchRootViewMode(runtime::root_view_mode::power);
}

void
RootView::onSubscribe()
{
    observe(runtime::selectors::select_root_view_mode,
            [this](runtime::root_view_mode mode) {
                setMode(to_underlying(mode));
            });
}

void
RootView::switchRootViewMode(runtime::root_view_mode mode)
{
    runtime::actions::set_root_view_mode action;
    action.mode = mode;
    dispatch(action);
}

} // namespace piejam::gui::model
