// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/EnumParameter.h>

#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>

#include <fmt/format.h>

namespace piejam::gui::model
{

struct EnumParameter::Impl
{
    Impl(std::vector<std::pair<std::string, int>> const& values)
        : values{values}
    {
    }

    EnumListModel values;
};

EnumParameter::EnumParameter(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        ParameterId const& param)
    : IntParameter{store_dispatch, state_change_subscriber, param}
    , m_impl{std::make_unique<Impl>(observe_once(
              runtime::selectors::make_int_parameter_enum_values_selector(
                      std::get<runtime::int_parameter_id>(param))))}

{
}

EnumParameter::~EnumParameter() = default;

auto
EnumParameter::values() const noexcept -> EnumListModel*
{
    return &m_impl->values;
}

} // namespace piejam::gui::model
