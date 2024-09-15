// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/tuple.h>

#include <nlohmann/json.hpp>

#include <boost/assert.hpp>
#include <boost/hof/match.hpp>

#include <optional>
#include <stdexcept>
#include <string_view>
#include <variant>

namespace piejam::runtime::persistence
{

namespace detail
{

template <class T>
struct variant_option_to_json_serializer
{
    std::string_view name;

    auto operator()(T const& x) const
    {
        return nlohmann::json{{name, x}};
    }
};

static inline auto const to_json_serializer_fail_option =
        [](auto&&) -> nlohmann::json {
    BOOST_ASSERT_MSG(false, "unknown variant type");
    throw std::runtime_error{"unable to serialize variant type"};
};

template <class T>
struct variant_option_from_json_serializer
{
    std::string_view name;

    auto operator()(nlohmann::json const& j) const -> std::optional<T>
    {
        if (j.contains(name))
        {
            T x;
            j[name].get_to(x);
            return x;
        }

        return std::nullopt;
    }
};

} // namespace detail

template <class T>
struct variant_option
{
    std::string_view name;

    auto to_json() const
    {
        return detail::variant_option_to_json_serializer<T>{name};
    }

    auto from_json() const
    {
        return detail::variant_option_from_json_serializer<T>{name};
    }
};

template <class... T>
struct variant_serializer
{
    using to_json_serializer_t = decltype(boost::hof::match(
            std::declval<detail::variant_option_to_json_serializer<T>>()...,
            detail::to_json_serializer_fail_option));

    using from_json_serializer_t =
            std::tuple<detail::variant_option_from_json_serializer<T>...>;

    explicit variant_serializer(variant_option<T>... opts)
        : to_json_serializer{boost::hof::match(
                  opts.to_json()...,
                  detail::to_json_serializer_fail_option)}
        , from_json_serializer{opts.from_json()...}
    {
    }

    template <class... Vs>
    auto to_json(std::variant<Vs...> const& var) const
    {
        return std::visit(to_json_serializer, var);
    }

    template <class... Vs>
    auto from_json(nlohmann::json const& j, std::variant<Vs...>& var) const
    {
        bool const found = tuple::for_each_until(
                from_json_serializer,
                [&](auto const& ser) {
                    if (auto const& opt = ser(j); opt)
                    {
                        var = *opt;
                        return true;
                    }

                    return false;
                });

        if (!found)
        {
            BOOST_ASSERT(false);
            throw std::runtime_error("unknown variant value type");
        }
    }

    to_json_serializer_t to_json_serializer;
    from_json_serializer_t from_json_serializer;
};

template <class... T>
variant_serializer(variant_option<T>...) -> variant_serializer<T...>;

} // namespace piejam::runtime::persistence
