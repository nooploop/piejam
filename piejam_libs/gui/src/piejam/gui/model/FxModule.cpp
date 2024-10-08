// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/FxModule.h>

#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct FxModule::Impl
{
    box<runtime::fx::module_parameters> parameters;
    box<runtime::fx::module_streams> streams;
};

FxModule::FxModule(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::fx::module_id const fx_mod_id)
    : Subscribable{store_dispatch, state_change_subscriber}
    , m_impl{make_pimpl<Impl>(
              observe_once(
                      runtime::selectors::make_fx_module_parameters_selector(
                              fx_mod_id)),
              observe_once(runtime::selectors::make_fx_module_streams_selector(
                      fx_mod_id)))}
    , m_busType{toBusType(observe_once(
              runtime::selectors::make_fx_module_bus_type_selector(fx_mod_id)))}
{
}

auto
FxModule::busType() const noexcept -> busType_property_t
{
    return m_busType;
}

auto
FxModule::parameters() const -> runtime::fx::module_parameters const&
{
    return m_impl->parameters;
}

auto
FxModule::streams() const -> runtime::fx::module_streams const&
{
    return m_impl->streams;
}

} // namespace piejam::gui::model
