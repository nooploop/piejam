// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

#include <memory>

namespace piejam::runtime
{

template <class Parameter>
struct parameter_map_slot
{
    using parameter_type = Parameter;

    Parameter param;

    struct value_slot
    {
        using value_type = parameter::value_type_t<Parameter>;
        using cached_type = std::shared_ptr<value_type const>;

        value_slot() = default;

        explicit value_slot(value_type value)
            : m_value{std::make_shared<value_type>(value)}
        {
        }

        auto get() const -> value_type
        {
            return *m_value;
        }

        void set(value_type value)
        {
            *m_value = value;
        }

        auto cached() const -> cached_type
        {
            return m_value;
        }

        auto operator==(value_slot const& other) const -> bool
        {
            return get() == other.get();
        }

        auto operator!=(value_slot const& other) const -> bool
        {
            return get() != other.get();
        }

    private:
        std::shared_ptr<value_type> m_value{
                std::make_shared<value_type>(value_type{})};
    } value{get_default_value(param)};

    auto operator==(parameter_map_slot const&) const -> bool = default;

private:
    auto get_default_value(Parameter const& param)
            -> parameter::value_type_t<Parameter>
    {
        if constexpr (parameter::has_default_value<Parameter>)
        {
            return param.default_value;
        }
        else
        {
            return {};
        }
    }
};

using parameters_map = parameter::map<parameter_map_slot>;

} // namespace piejam::runtime
