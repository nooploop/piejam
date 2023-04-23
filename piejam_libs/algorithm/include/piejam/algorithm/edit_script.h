// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/algorithm/for_each_visit.h>

#include <algorithm>
#include <ranges>
#include <type_traits>
#include <variant>
#include <vector>

namespace piejam::algorithm
{

struct edit_script_deletion
{
    std::size_t pos{};

    [[nodiscard]] constexpr auto
    operator==(edit_script_deletion const&) const noexcept -> bool = default;
};

template <class T>
struct edit_script_insertion
{
    std::size_t pos{};
    T value{};

    [[nodiscard]] constexpr auto
    operator==(edit_script_insertion const&) const noexcept -> bool = default;
};

template <class T>
edit_script_insertion(std::size_t, T const&) -> edit_script_insertion<T>;

template <class T>
using edit_script_op =
        std::variant<edit_script_deletion, edit_script_insertion<T>>;

template <class T>
using edit_script_ops = std::vector<edit_script_op<T>>;

template <class... Range>
using common_range_value_type_t =
        std::common_type_t<std::ranges::range_value_t<Range>...>;

template <class Src, class Dst>
[[nodiscard]] auto
edit_script(Src const& src, Dst const& dst)
        -> edit_script_ops<common_range_value_type_t<Src, Dst>>
{
    using value_type = common_range_value_type_t<Src, Dst>;

    edit_script_ops<value_type> result;
    result.reserve(src.size() + dst.size());

    if (src.size() == 0 && dst.size() > 0)
    {
        std::ranges::transform(
                dst,
                std::back_inserter(result),
                [i = std::size_t{}](auto const& v) mutable {
                    return edit_script_insertion{i++, v};
                });
    }
    else if (src.size() > 0 && dst.size() == 0)
    {
        std::generate_n(
                std::back_inserter(result),
                src.size(),
                [i = std::size_t{}]() mutable {
                    return edit_script_deletion{i++};
                });
    }
    else if (src.size() > 0 && dst.size() > 0)
    {
        std::vector<std::size_t> graph((src.size() + 1) * (dst.size() + 1));

        auto const row_size = src.size() + 1;

        for (std::size_t y = 0; y < row_size; ++y)
        {
            graph[y] = y;
        }

        for (std::size_t y = 0, p = 0; y < dst.size() + 1; ++y, p += row_size)
        {
            graph[p] = y;
        }

        auto left = [](auto* g) { return g - 1; };
        auto up = [row_size](auto* g) { return g - row_size; };

        auto* g = graph.data() + row_size + 1;
        for (std::size_t y = 1; y <= dst.size(); ++y, ++g)
        {
            for (std::size_t x = 1; x <= src.size(); ++x, ++g)
            {
                if (src[x - 1] == dst[y - 1])
                {
                    *g = *up(left(g));
                }
                else
                {
                    *g = std::min(*left(g), *up(g)) + 1;
                }
            }
        }

        auto move_left = [](auto* g, auto x) {
            return std::pair(g - 1, x - 1);
        };
        auto move_up = [row_size](auto* g, auto y) {
            return std::pair(g - row_size, y - 1);
        };

        std::size_t x = src.size(), y = dst.size();
        g = graph.data() + graph.size() - 1;
        while (x > 0 || y > 0)
        {
            if (x > 0 && y > 0)
            {
                if (src[x - 1] == dst[y - 1])
                {
                    std::tie(g, x) = move_left(g, x);
                    std::tie(g, y) = move_up(g, y);
                }
                else
                {
                    if (*up(g) <= *left(g))
                    {
                        result.emplace_back(
                                edit_script_insertion{y - 1, dst[y - 1]});
                        std::tie(g, y) = move_up(g, y);
                    }
                    else
                    {
                        result.emplace_back(edit_script_deletion{x - 1});
                        std::tie(g, x) = move_left(g, x);
                    }
                }
            }
            else if (x == 0)
            {
                result.emplace_back(edit_script_insertion{y - 1, dst[y - 1]});
                std::tie(g, y) = move_up(g, y);
            }
            else if (y == 0)
            {
                result.emplace_back(edit_script_deletion{x - 1});
                std::tie(g, x) = move_left(g, x);
            }
        }

        std::ranges::reverse(result);
    }

    return result;
}

namespace detail
{

template <class T>
[[nodiscard]] auto
prepare_edit_script_ops_for_linear_execution(edit_script_ops<T> ops)
        -> edit_script_ops<T>
{
    struct
    {
        std::size_t deletion_offset{};

        auto operator()(edit_script_deletion& del)
        {
            del.pos -= deletion_offset++;
        }

        auto operator()(edit_script_insertion<T>&)
        {
            --deletion_offset;
        }
    } visitor;

    for_each_visit(ops, visitor);

    return ops;
}

} // namespace detail

template <class T, class Visitor>
[[nodiscard]] auto
apply_edit_script(edit_script_ops<T> ops, Visitor&& v)
{
    for_each_visit(
            detail::prepare_edit_script_ops_for_linear_execution(
                    std::move(ops)),
            std::forward<Visitor>(v));
}

} // namespace piejam::algorithm
