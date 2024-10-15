// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/FxModuleType.h>
#include <piejam/gui/model/Subscribable.h>
#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/Types.h>
#include <piejam/gui/model/fwd.h>

#include <piejam/pimpl.h>
#include <piejam/runtime/fx/fwd.h>

namespace piejam::gui::model
{

class FxModule : public Subscribable<SubscribableModel>
{
    Q_OBJECT

    Q_PROPERTY(piejam::gui::model::FxModuleType type READ type CONSTANT FINAL)
    M_PIEJAM_GUI_CONSTANT_PROPERTY(piejam::gui::model::BusType, busType)

public:
    FxModule(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::fx::module_id);

    virtual auto type() const noexcept -> FxModuleType = 0;

protected:
    auto parameters() const -> runtime::fx::module_parameters const&;
    auto streams() const -> runtime::fx::module_streams const&;

private:
    struct Impl;
    pimpl<Impl> m_impl;

    BusType m_busType;
};

} // namespace piejam::gui::model
