// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/parameter/fwd.h>

#include <memory>

namespace piejam::runtime
{

template <class Parameter>
struct parameter_descriptor
{
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

    private:
        std::shared_ptr<value_type> m_value{
                std::make_shared<value_type>(value_type{})};
    } value{param.default_value};
};

using parameters_map = parameter::map<parameter_descriptor>;

} // namespace piejam::runtime
