// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/gui/model/fwd.h>
#include <piejam/registry_map.h>
#include <piejam/runtime/fwd.h>
#include <piejam/runtime/fx/fwd.h>
#include <piejam/runtime/store_dispatch.h>
#include <piejam/runtime/subscriber.h>

#include <functional>
#include <memory>

namespace piejam::gui::model
{

using FxModuleContentFactory = std::function<std::unique_ptr<FxModuleContent>(
        runtime::store_dispatch,
        runtime::subscriber&,
        runtime::fx::module_id)>;

using FxModuleContentFactories = registry_map<
        FxModuleContentFactory,
        runtime::fx::internal_id,
        FxModuleContentFactory>;

template <class T>
auto
makeFxModuleContent(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& subscriber,
        runtime::fx::module_id fx_mod_id) -> std::unique_ptr<FxModuleContent>
{
    return std::make_unique<T>(store_dispatch, subscriber, fx_mod_id);
}

} // namespace piejam::gui::model
