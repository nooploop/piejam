// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/level_meter.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/components/remap_channels.h>
#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/graph_generic_algorithms.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/pan_balance_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/pair.h>

#include <fmt/format.h>

#include <array>

namespace piejam::audio::components
{

namespace
{

class pan_balance final : public engine::component
{
public:
    explicit pan_balance(
            std::unique_ptr<engine::processor> param_proc,
            std::unique_ptr<engine::component> amp_comp)
        : m_param_proc(std::move(param_proc))
        , m_amp_comp(std::move(amp_comp))
    {
    }

    [[nodiscard]] auto inputs() const -> endpoints override
    {
        return m_amp_comp->inputs();
    }

    [[nodiscard]] auto outputs() const -> endpoints override
    {
        return m_amp_comp->outputs();
    }

    [[nodiscard]] auto event_inputs() const -> endpoints override
    {
        return m_event_inputs;
    }

    [[nodiscard]] auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(engine::graph& g) const override
    {
        m_amp_comp->connect(g);

        using namespace engine::endpoint_ports;
        engine::connect_event(
                g,
                *m_param_proc,
                from<0, 1>,
                *m_amp_comp,
                to<0, 1>);
    }

private:
    std::unique_ptr<engine::processor> m_param_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::vector<engine::graph_endpoint> m_event_inputs{
            engine::in_event_endpoints(*m_param_proc)};
};

} // namespace

auto
make_pan(
        std::unique_ptr<engine::processor> pan_converter,
        std::string_view name) -> std::unique_ptr<engine::component>
{
    using namespace engine::endpoint_ports;
    return std::make_unique<pan_balance>(
            std::move(pan_converter),
            make_remap_input_channels(
                    make_stereo_split_amplifier(fmt::format("pan {}", name)),
                    to<0, 1>));
}

auto
make_balance(
        std::unique_ptr<engine::processor> balance_converter,
        std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<pan_balance>(
            std::move(balance_converter),
            make_stereo_split_amplifier(fmt::format("balance {}", name)));
}

} // namespace piejam::audio::components
