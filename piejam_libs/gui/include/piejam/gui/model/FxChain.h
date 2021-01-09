// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/GenericListModel.h>
#include <piejam/gui/model/StringList.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

namespace piejam::gui::model
{

class FxChain : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QAbstractListModel* modules READ modules CONSTANT)

public:
    auto modules() noexcept -> FxModulesList* { return &m_modules; }

private:
    FxModulesList m_modules;
};

} // namespace piejam::gui::model
