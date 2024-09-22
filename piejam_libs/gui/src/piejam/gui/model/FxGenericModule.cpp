// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxGenericModule.h>

#include <piejam/gui/model/Parameter.h>
#include <piejam/gui/model/ParameterId.h>
#include <piejam/runtime/selectors.h>

#include <boost/container/flat_map.hpp>

namespace piejam::gui::model
{

struct FxGenericModule::Impl
{
    FxParametersList parametersList;
};

FxGenericModule::FxGenericModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : FxModule{store_dispatch, state_change_subscriber, fx_mod_id}
    , m_impl{std::make_unique<Impl>()}
{
    for (auto const& [key, paramId] : parameters())
    {
        auto param = model::makeParameter(
                dispatch(),
                state_change_subscriber,
                paramId);
        m_impl->parametersList.add(
                m_impl->parametersList.size(),
                std::move(param));
    }
}

FxGenericModule::~FxGenericModule() = default;

auto
FxGenericModule::parametersList() noexcept -> FxParametersList*
{
    return &m_impl->parametersList;
}

void
FxGenericModule::onSubscribe()
{
}

} // namespace piejam::gui::model
