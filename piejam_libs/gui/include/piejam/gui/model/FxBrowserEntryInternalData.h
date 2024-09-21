// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/registry_map.h>
#include <piejam/runtime/fx/fwd.h>

#include <QString>

namespace piejam::gui::model
{

struct FxBrowserEntryInternalData
{
    QString name;
    QString description;
};

using FxBrowserEntryInternalDataMap =
        registry_map<runtime::fx::internal_id, FxBrowserEntryInternalData>;

} // namespace piejam::gui::model
