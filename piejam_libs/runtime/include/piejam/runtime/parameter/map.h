// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/entity_map.h>
#include <piejam/runtime/parameter/fwd.h>
#include <piejam/tuple_element_compare.h>

#include <boost/container/flat_map.hpp>

#include <algorithm>
#include <concepts>
#include <memory>
#include <vector>

namespace piejam::runtime::parameter
{

template <class Parameter>
class map
{
public:
    using id_t = typename entity_map<Parameter>::id_t;
    using value_type = typename Parameter::value_type;

    using cached_type = std::shared_ptr<value_type>;
    using const_cached_type = std::shared_ptr<value_type const>;
    using cache_t = std::vector<cached_type>;

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        return m_parameters.empty();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        return m_parameters.size();
    }

    template <std::same_as<Parameter> P>
    auto add(P&& p) -> id_t
    {
        return add(std::forward<P>(p), p.default_value);
    }

    template <std::same_as<Parameter> P, std::convertible_to<value_type> V>
    auto add(P&& p, V&& value) -> id_t
    {
        auto id = m_parameters.add(std::forward<P>(p));
        auto&& [it, inserted] = m_values.emplace(id, std::forward<V>(value));
        BOOST_ASSERT(inserted);
        m_cache->insert(
                std::next(m_cache->begin(), m_values.index_of(it)),
                std::make_shared<value_type>(it->second));
        return id;
    }

    auto remove(id_t const id) -> void
    {
        m_parameters.remove(id);
        auto it = m_values.find(id);
        m_cache->erase(std::next(m_cache->begin(), m_values.index_of(it)));
        m_values.erase(it);
    }

    auto contains(id_t const id) const noexcept -> bool
    {
        return m_parameters.contains(id);
    }

    auto get_parameter(id_t const id) const noexcept -> Parameter const*
    {
        return m_parameters.find(id);
    }

    auto find(id_t const id) const noexcept -> value_type const*
    {
        auto it = m_values.find(id);
        return it != m_values.end() ? std::addressof(it->second) : nullptr;
    }

    auto get(id_t const id) const noexcept -> value_type const&
    {
        auto it = m_values.find(id);
        BOOST_ASSERT(it != m_values.end());
        return it->second;
    }

    auto get_cached(id_t const id) const noexcept -> const_cached_type
    {
        auto it = m_values.find(id);
        return it != m_values.end() ? (*m_cache)[m_values.index_of(it)]
                                    : nullptr;
    }

    template <std::convertible_to<value_type> V>
    auto set(id_t const id, V&& value) -> void
    {
        auto it = m_values.find(id);
        BOOST_ASSERT(it != m_values.end());
        it->second = std::forward<V>(value);

        *(*m_cache)[m_values.index_of(it)] = it->second;
    }

    auto set(id_value_map_t<Parameter> const& id_values)
    {
        auto it = m_values.begin();
        for (auto&& [id, value] : id_values)
        {
            it = std::ranges::find_if(
                    it,
                    m_values.end(),
                    tuple::element<0>.equal_to(id));
            BOOST_ASSERT(it != m_values.end());
            it->second = value;

            *(*m_cache)[m_values.index_of(it)] = it->second;
        }
    }

private:
    entity_map<Parameter> m_parameters;
    id_value_map_t<Parameter> m_values;
    std::shared_ptr<cache_t> m_cache{std::make_shared<cache_t>()};
};

} // namespace piejam::runtime::parameter
