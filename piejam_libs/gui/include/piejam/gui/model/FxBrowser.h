// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/FxBrowserEntry.h>
#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxBrowser : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* entries READ entries CONSTANT)

public:
    auto entries() noexcept -> FxBrowserList* { return &m_entries; }

private:
    FxBrowserList m_entries;
};

} // namespace piejam::gui::model
