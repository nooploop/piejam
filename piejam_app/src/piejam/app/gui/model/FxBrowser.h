// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/app/gui/model/Subscribable.h>
#include <piejam/gui/model/FxBrowser.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/fx/registry.h>

namespace piejam::app::gui::model
{

class FxBrowser final : public Subscribable<piejam::gui::model::FxBrowser>
{
public:
    FxBrowser(runtime::store_dispatch, runtime::subscriber&);

private:
    void onSubscribe() override;

    auto makeBrowserEntry(runtime::fx::internal)
            -> std::unique_ptr<piejam::gui::model::FxBrowserEntry>;
    auto makeBrowserEntry(audio::ladspa::plugin_descriptor const&)
            -> std::unique_ptr<piejam::gui::model::FxBrowserEntry>;

    runtime::fx::registry m_fx_registry;
};

} // namespace piejam::app::gui::model
