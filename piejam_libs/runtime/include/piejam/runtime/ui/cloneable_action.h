// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>

namespace piejam::runtime::ui
{

template <class DerivedAction, class ActionInterface>
struct cloneable_action : public ActionInterface
{
    auto clone() const -> std::unique_ptr<ActionInterface> override
    {
        return std::make_unique<DerivedAction>(static_cast<DerivedAction const&>(*this));
    }
};

} // namespace piejam::runtime::ui
