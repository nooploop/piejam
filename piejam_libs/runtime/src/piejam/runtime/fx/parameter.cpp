// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
