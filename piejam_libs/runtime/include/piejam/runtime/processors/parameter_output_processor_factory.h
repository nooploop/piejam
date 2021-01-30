// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/value_output_processor.h>
#include <piejam/entity_id_hash.h>
#include <piejam/runtime/parameter/fwd.h>
#include <piejam/runtime/parameter/map.h>

#include <boost/assert.hpp>

#include <concepts>
#include <memory>
#include <string_view>
#include <tuple>
#include <unordered_map>

namespace piejam::runtime::processors
{

template <class... Parameter>
class parameter_output_processor_factory
{
public:
    template <class P>
    using parameter_processor =
            audio::engine::value_output_processor<typename P::value_type>;

    template <class P>
    using processor_map = std::unordered_map<
            parameter::id_t<P>,
            std::weak_ptr<parameter_processor<P>>>;

    template <class P>
    auto
    make_processor(parameter::id_t<P> id, std::string_view const& name = {})
            -> std::shared_ptr<parameter_processor<P>>
    {
        auto proc = std::make_shared<parameter_processor<P>>(name);
        BOOST_VERIFY(
                std::get<processor_map<P>>(m_procs).emplace(id, proc).second);
        return proc;
    }

    template <class P, class F>
    auto consume(parameter::id_t<P> id, F&& f) const
    {
        auto& proc_map = std::get<processor_map<P>>(m_procs);
        auto it = proc_map.find(id);
        if (it != proc_map.end())
        {
            if (auto proc = it->second.lock())
                proc->consume(std::forward<F>(f));
        }
    }

    void clear_expired() { (..., clear_expired<Parameter>()); }

private:
    template <class P>
    void clear_expired()
    {
        std::erase_if(std::get<processor_map<P>>(m_procs), [](auto const& p) {
            return p.second.expired();
        });
    }

    std::tuple<processor_map<Parameter>...> m_procs;
};

} // namespace piejam::runtime::processors
