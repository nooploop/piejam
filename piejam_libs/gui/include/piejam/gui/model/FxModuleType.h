// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/runtime/fx/fwd.h>

#include <QObject>

namespace piejam::gui::model
{

struct FxModuleType
{
    Q_GADGET

public:
    auto operator==(FxModuleType const& other) const noexcept -> bool = default;
    auto operator!=(FxModuleType const& other) const noexcept -> bool = default;

    runtime::fx::internal_id id;
};

} // namespace piejam::gui::model

Q_DECLARE_METATYPE(piejam::gui::model::FxModuleType)
