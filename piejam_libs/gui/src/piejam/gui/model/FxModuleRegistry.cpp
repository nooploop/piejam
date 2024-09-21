// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxModuleRegistry.h>

#include <piejam/algorithm/index_of.h>

#include <memory>

namespace piejam::gui::model
{

void
FxModuleRegistry::addItem(FxModuleType fxType, QString viewSource)
{
    m_items.push_back(
            {.fxType = std::move(fxType), .viewSource = std::move(viewSource)});
}

int
FxModuleRegistry::indexOf(FxModuleType fxType)
{
    auto index =
            algorithm::index_of(m_items, fxType, &FxModuleRegistryItem::fxType);
    return static_cast<int>(index);
}

auto
fxModuleRegistrySingleton() -> FxModuleRegistry&
{
    static std::unique_ptr<FxModuleRegistry> s_registry{[]() {
        auto r = std::make_unique<FxModuleRegistry>();
        r->addItem(
                FxModuleType{},
                "/PieJam/FxChainControls/ParametersListView.qml");
        return r;
    }()};
    return *s_registry;
}

} // namespace piejam::gui::model
