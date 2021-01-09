// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/fx/parameter.h>

#include <fmt/format.h>

#include <boost/assert.hpp>

#include <concepts>

namespace piejam::runtime::fx
{

parameter_value_to_string::parameter_value_to_string() noexcept
    : m_to_string([](float x) { return fmt::format("{:.2f}", x); })
{
}

parameter_value_to_string::parameter_value_to_string(float_to_string f) noexcept
    : m_to_string(f)
{
    BOOST_ASSERT(f);
}

parameter_value_to_string::parameter_value_to_string(int_to_string f) noexcept
    : m_to_string(f)
{
    BOOST_ASSERT(f);
}

parameter_value_to_string::parameter_value_to_string(bool_to_string f) noexcept
    : m_to_string(f)
{
    BOOST_ASSERT(f);
}

namespace
{

template <class T>
struct to_string_visitor
{
    template <class X>
    using to_string_f = std::string (*)(X);

    auto operator()(to_string_f<T> f) const -> std::string
    {
        BOOST_ASSERT(f);
        return f(value);
    }

    template <class U>
    auto operator()(to_string_f<U>) const -> std::string
    {
        return std::to_string(value);
    }

    T value;
};

template <class T>
to_string_visitor(T) -> to_string_visitor<T>;

} // namespace

auto
parameter_value_to_string::operator()(float x) const -> std::string
{
    return std::visit(to_string_visitor{x}, m_to_string);
}

auto
parameter_value_to_string::operator()(int x) const -> std::string
{
    return std::visit(to_string_visitor{x}, m_to_string);
}

auto
parameter_value_to_string::operator()(bool x) const -> std::string
{
    return std::visit(to_string_visitor{x}, m_to_string);
}

} // namespace piejam::runtime::fx
