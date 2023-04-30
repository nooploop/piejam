// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/level_meter.h>

#include <piejam/audio/components/amplifier.h>
#include <piejam/audio/engine/component.h>
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
                from<0, 1>{},
                *m_amp_comp,
                to<0, 1>{});
    }

private:
    std::unique_ptr<engine::processor> m_param_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::array<engine::graph_endpoint, 1> m_event_inputs{
            engine::graph_endpoint{.proc = *m_param_proc, .port = 0}};
};

class volume_pan final : public engine::component
{
public:
    explicit volume_pan(std::string_view const name)
        : m_param_proc(engine::make_volume_pan_processor(name))
        , m_amp_comp(make_stereo_split_amplifier(
                  fmt::format("volume_pan {}", name)))
    {
    }

    [[nodiscard]] auto inputs() const -> endpoints override
    {
        return m_inputs;
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
                from<0, 1>{},
                *m_amp_comp,
                to<0, 1>{});

        engine::connect(
                g,
                *m_input_proc,
                from<0, 0>{},
                *m_amp_comp,
                to<0, 1>{});
    }

private:
    std::unique_ptr<engine::processor> m_param_proc;
    std::unique_ptr<engine::component> m_amp_comp;
    std::unique_ptr<engine::processor> m_input_proc{
            engine::make_identity_processor()};

    std::array<engine::graph_endpoint, 1> m_inputs{
            engine::graph_endpoint{.proc = *m_input_proc, .port = 0}};
    std::array<engine::graph_endpoint, 2> m_event_inputs{
            engine::graph_endpoint{.proc = *m_param_proc, .port = 0},
            engine::graph_endpoint{.proc = *m_param_proc, .port = 1}};
};

class volume_stereo_balance final : public engine::component
{
public:
    explicit volume_stereo_balance(std::string_view const name)
        : m_param_proc(engine::make_volume_stereo_balance_processor(name))
        , m_amp_comp(make_stereo_split_amplifier(
                  fmt::format("volume_balance {}", name)))
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
                from<0, 1>{},
                *m_amp_comp,
                to<0, 1>{});
    }

private:
    std::unique_ptr<engine::processor> m_param_proc;
    std::unique_ptr<engine::component> m_amp_comp;

    std::array<engine::graph_endpoint, 2> m_event_inputs{
            engine::graph_endpoint{.proc = *m_param_proc, .port = 0},
            engine::graph_endpoint{.proc = *m_param_proc, .port = 1}};
};

} // namespace

auto
make_pan(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<pan_balance>(
            engine::make_pan_processor(name),
            make_stereo_split_amplifier(fmt::format("pan {}", name)));
}

auto
make_volume_pan(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<volume_pan>(name);
}

auto
make_stereo_balance(std::string_view name) -> std::unique_ptr<engine::component>
{
    return std::make_unique<pan_balance>(
            engine::make_stereo_balance_processor(name),
            make_stereo_split_amplifier(fmt::format("balance {}", name)));
}

auto
make_volume_stereo_balance(std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<volume_stereo_balance>(name);
}

} // namespace piejam::audio::components
