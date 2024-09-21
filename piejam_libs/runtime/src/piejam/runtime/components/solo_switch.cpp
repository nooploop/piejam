// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/solo_switch.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/named_processor.h>
#include <piejam/audio/engine/verify_process_context.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>
#include <piejam/runtime/parameter_processor_factory.h>

#include <fmt/format.h>

#include <boost/dynamic_bitset.hpp>

#include <array>
#include <vector>

namespace piejam::runtime::components
{

namespace
{

class solo_switch_processor final : public audio::engine::named_processor
{
public:
    solo_switch_processor(
            solo_groups_t const& solo_groups,
            std::string_view const name = {})
        : named_processor(name)
        , m_solo_group_state{solo_groups}
    {
    }

    auto type_name() const noexcept -> std::string_view override
    {
        return "solo_switch";
    }

    auto num_inputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    auto event_inputs() const noexcept -> event_ports override
    {
        return m_event_inputs;
    }

    auto event_outputs() const noexcept -> event_ports override
    {
        return m_event_outputs;
    }

    void process(audio::engine::process_context const& ctx) override
    {
        audio::engine::verify_process_context(*this, ctx);

        bool send_update{};
        for (std::size_t const in_index : range::indices(ctx.event_inputs))
        {
            auto const& ev_buf = ctx.event_inputs.get<bool>(in_index);
            for (auto const& ev : ev_buf)
            {
                m_solo_group_state.set_solo(in_index, ev.value());
                send_update = true;
            }
        }

        if (!send_update)
        {
            return;
        }

        if (m_solo_group_state.any_solo())
        {
            m_solo_group_state.calculate_mutes();

            for (std::size_t const index : range::indices(ctx.event_outputs))
            {
                auto& ev_buf = ctx.event_outputs.get<bool>(index);
                ev_buf.insert(0, m_solo_group_state.mutes().test(index));
            }
        }
        else
        {
            for (std::size_t const index : range::indices(ctx.event_outputs))
            {
                auto& ev_buf = ctx.event_outputs.get<bool>(index);
                ev_buf.insert(0, false);
            }
        }
    }

private:
    solo_group_state m_solo_group_state;

    std::vector<audio::engine::event_port> m_event_inputs{
            algorithm::transform_to_vector(
                    range::iota(m_solo_group_state.mutes().size()),
                    [](std::size_t const member_index) {
                        return audio::engine::event_port(
                                std::in_place_type<bool>,
                                fmt::format("in{}", member_index));
                    })};

    std::vector<audio::engine::event_port> m_event_outputs{
            algorithm::transform_to_vector(
                    range::iota(m_solo_group_state.mutes().size()),
                    [](std::size_t const member_index) {
                        return audio::engine::event_port(
                                std::in_place_type<bool>,
                                fmt::format("out{}", member_index));
                    })};
};

class solo_switch final : public audio::engine::component
{
public:
    solo_switch(
            solo_groups_t const& solo_groups,
            parameter_processor_factory& param_procs,
            std::string_view const name)
        : m_solo_inputs(make_solo_inputs(solo_groups, param_procs))
        , m_solo_switch_proc(
                  std::make_unique<solo_switch_processor>(solo_groups, name))
    {
    }

    auto inputs() const -> endpoints override
    {
        return {};
    }

    auto outputs() const -> endpoints override
    {
        return {};
    }

    auto event_inputs() const -> endpoints override
    {
        return {};
    }

    auto event_outputs() const -> endpoints override
    {
        return m_event_outputs;
    }

    void connect(audio::engine::graph& g) const override
    {
        for (std::size_t const index : range::indices(m_solo_inputs))
        {
            g.event.insert(
                    audio::engine::graph_endpoint{
                            .proc = *m_solo_inputs[index],
                            .port = 0},
                    audio::engine::graph_endpoint{
                            .proc = *m_solo_switch_proc,
                            .port = index});
        }
    }

private:
    static auto make_solo_inputs(
            solo_groups_t const& solo_groups,
            parameter_processor_factory& param_procs)
            -> std::vector<std::shared_ptr<audio::engine::processor>>
    {
        std::vector<std::shared_ptr<audio::engine::processor>> result;

        for (auto const& [channel_id, group] : solo_groups)
        {
            result.emplace_back(
                    param_procs.make_processor(group.solo_param, "solo"));
        }

        return result;
    }

    std::vector<std::shared_ptr<audio::engine::processor>> m_solo_inputs;
    std::unique_ptr<audio::engine::processor> m_solo_switch_proc;

    std::vector<audio::engine::graph_endpoint> m_event_outputs{
            algorithm::transform_to_vector(
                    range::indices(m_solo_switch_proc->event_outputs()),
                    [this](auto const index) {
                        return audio::engine::graph_endpoint{
                                .proc = *m_solo_switch_proc,
                                .port = index};
                    })};
};

} // namespace

auto
make_solo_switch(
        solo_groups_t const& solo_group,
        parameter_processor_factory& param_procs,
        std::string_view const name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<solo_switch>(solo_group, param_procs, name);
}

} // namespace piejam::runtime::components
