// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/runtime/processors/parameter_input_processor_factory.h>
#include <piejam/runtime/processors/parameter_output_processor_factory.h>

#include <tuple>
#include <variant>

namespace piejam::runtime::processors
{

template <class, class>
class parameter_processor_factory;

template <class... InParameter, class... OutParameter>
class parameter_processor_factory<
        std::tuple<InParameter...>,
        std::tuple<OutParameter...>>
{
public:
    template <class P>
    auto make_input_processor(
            parameter::id_t<P> id,
            std::string_view const& name = {})
    {
        return m_in_factory.template make_processor<P>(id, name);
    }

    template <class P>
    auto make_output_processor(
            parameter::id_t<P> id,
            std::string_view const& name = {})
    {
        return m_out_factory.template make_processor<P>(id, name);
    }

    template <class P>
    void initialize(parameter::map<P> const& params) const
    {
        m_in_factory.template initialize<P>(params);
    }

    template <class... Ps>
    void initialize(parameter::maps_collection<Ps...> const& params) const
    {
        m_in_factory.initialize(params);
    }

    template <class P, std::convertible_to<typename P::value_type> V>
    void set(parameter::id_t<P> id, V&& value) const
    {
        m_in_factory.template set<P>(id, std::forward<V>(value));
    }

    template <class P, class F>
    auto consume(parameter::id_t<P> id, F&& f) const
    {
        m_out_factory.template consume<P, F>(id, std::forward<F>(f));
    }

    void clear_expired()
    {
        m_in_factory.clear_expired();
        m_out_factory.clear_expired();
    }

private:
    parameter_input_processor_factory<InParameter...> m_in_factory;
    parameter_output_processor_factory<OutParameter...> m_out_factory;
};

template <class ProcessorFactory, class... P>
auto
make_input_processor(
        ProcessorFactory& proc_factory,
        std::variant<P...> const& param,
        std::string_view const& name = {})
        -> std::shared_ptr<audio::engine::processor>
{
    return std::visit(
            [&proc_factory,
             &name](auto&& p) -> std::shared_ptr<audio::engine::processor> {
                return proc_factory.make_input_processor(
                        std::forward<decltype(p)>(p),
                        name);
            },
            param);
}

} // namespace piejam::runtime::processors
