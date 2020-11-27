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
        auto& proc_map = std::get<processor_map<P>>(m_procs);
        for (auto it = proc_map.begin(); it != proc_map.end();)
        {
            if (it->second.expired())
                it = proc_map.erase(it);
            else
                ++it;
        }
    }

    std::tuple<processor_map<Parameter>...> m_procs;
};

} // namespace piejam::runtime::processors
