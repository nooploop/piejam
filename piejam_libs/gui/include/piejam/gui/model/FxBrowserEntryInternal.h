// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_id.h>
#include <piejam/gui/model/FxBrowserEntry.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::gui::model
{

class FxBrowserEntryInternal final : public FxBrowserEntry
{
public:
    FxBrowserEntryInternal(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::internal_id);

    void appendModule() override;

private:
    void onSubscribe() override;

    runtime::fx::internal_id m_fx_type;
};

} // namespace piejam::gui::model
