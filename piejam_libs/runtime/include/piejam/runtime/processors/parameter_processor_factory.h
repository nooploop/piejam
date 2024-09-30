// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/value_io_processor.h>
#include <piejam/entity_id_hash.h>
#include <piejam/runtime/parameter/fwd.h>

#include <boost/assert.hpp>
#include <boost/mp11/tuple.hpp>

#include <concepts>
#include <memory>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>

namespace piejam::runtime::processors
{

template <class... Parameter>
class parameter_processor_factory
{
public:
    template <class P>
    using parameter_processor =
            audio::engine::value_io_processor<typename P::value_type>;

    template <class P>
    using processor_map = std::unordered_map<
            parameter::id_t<P>,
            std::weak_ptr<parameter_processor<P>>>;

    template <class P>
    auto make_processor(parameter::id_t<P> id, std::string_view const name = {})
            -> std::shared_ptr<parameter_processor<P>>
    {
        BOOST_ASSERT(id.valid());
        auto proc = std::make_shared<parameter_processor<P>>(name);
        std::get<processor_map<P>>(m_procs).insert_or_assign(id, proc);
        return proc;
    }

    template <class P>
    auto find_processor(parameter::id_t<P> id) const
            -> std::shared_ptr<parameter_processor<P>>
    {
        auto const& map = std::get<processor_map<P>>(m_procs);
        auto it = map.find(id);
        return it != map.end() ? it->second.lock() : nullptr;
    }

    template <class FindValue>
    void initialize(FindValue&& find_value) const
    {
        boost::mp11::tuple_for_each(m_procs, [&](auto&& procs) {
            for (auto&& [id, weak_proc] : procs)
            {
                if (auto proc = weak_proc.lock())
                {
                    if (auto const value = find_value(id); value)
                    {
                        proc->set(*value);
                    }
                }
            }
        });
    }

    template <class P, std::convertible_to<typename P::value_type> V>
    void set(parameter::id_t<P> id, V&& value) const
    {
        if (auto proc = find_processor(id))
        {
            proc->set(std::forward<V>(value));
        }
    }

    template <class P, class F>
    auto consume(parameter::id_t<P> id, F&& f) const
    {
        if (auto proc = find_processor(id))
        {
            proc->consume(std::forward<F>(f));
        }
    }

    bool has_expired() const noexcept
    {
        return (has_expired<Parameter>() || ...);
    }

    void clear_expired()
    {
        (clear_expired<Parameter>(), ...);
    }

private:
    template <class P>
    static bool expired(typename processor_map<P>::value_type const& p) noexcept
    {
        return p.second.expired();
    }

    template <class P>
    bool has_expired() const noexcept
    {
        return std::ranges::any_of(
                std::get<processor_map<P>>(m_procs),
                &expired<P>);
    }

    template <class P>
    void clear_expired()
    {
        std::erase_if(std::get<processor_map<P>>(m_procs), &expired<P>);
    }

    std::tuple<processor_map<Parameter>...> m_procs;
};

template <class ProcessorFactory, class... P>
auto
make_parameter_processor(
        ProcessorFactory& proc_factory,
        std::variant<parameter::id_t<P>...> const& param,
        std::string_view const name = {})
        -> std::shared_ptr<audio::engine::processor>
{
    return std::visit(
            [&proc_factory,
             &name](auto&& p) -> std::shared_ptr<audio::engine::processor> {
                return proc_factory.make_processor(
                        std::forward<decltype(p)>(p),
                        name);
            },
            param);
}

} // namespace piejam::runtime::processors
