// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/amplifier.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/multiply_processor.h>
#include <piejam/audio/engine/processor.h>
#include <piejam/audio/engine/smoother_processor.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/range/iota.h>
#include <piejam/range/zip.h>

#include <fmt/format.h>

#include <boost/assert.hpp>
#include <boost/range/adaptor/indirected.hpp>

#include <array>

namespace piejam::audio::components
{

namespace
{

auto
format_name(
        std::string_view name,
        std::string_view param,
        std::size_t ch,
        std::size_t num_channels)
{
    switch (num_channels)
    {
        case 1:
            return fmt::format("{} {}", name, param);

        case 2:
            switch (ch)
            {
                case 0:
                    return fmt::format("{} {} L", name, param);
                case 1:
                    return fmt::format("{} {} R", name, param);
                default:
                    BOOST_ASSERT(false);
                    return fmt::format("{} {}", name, param);
            }

        default:
            return fmt::format("{} {} {}", name, param, ch);
    }
}

//! amplifies with same gain all channels
class amplifier final : public engine::component
{
public:
    amplifier(std::size_t num_channels, std::string_view name)
        : m_gain_proc{engine::make_event_to_audio_smoother_processor(
                  engine::default_smooth_length,
                  fmt::format("{} gain", name))}
        , m_amp_procs{algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [=](auto ch) {
                      return engine::make_multiply_processor(
                              2,
                              format_name(name, "amp", ch, num_channels));
                  })}
        , m_inputs{algorithm::transform_to_vector(
                  m_amp_procs | boost::adaptors::indirected,
                  engine::make_graph_endpoint<0>)}
        , m_outputs{m_inputs}
    {
    }

    [[nodiscard]] auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    [[nodiscard]] auto outputs() const -> endpoints override
    {
        return m_outputs;
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
        for (auto const& amp_proc : m_amp_procs)
        {
            g.audio.insert({*m_gain_proc, 0}, {*amp_proc, 1});
        }
    }

private:
    std::unique_ptr<engine::processor> m_gain_proc;
    std::vector<std::unique_ptr<engine::processor>> m_amp_procs;

    std::vector<engine::graph_endpoint> m_inputs;
    std::vector<engine::graph_endpoint> m_outputs;
    std::array<engine::graph_endpoint, 1> m_event_inputs{{{*m_gain_proc, 0}}};
};

//! amplifies every channel with it's own gain
class split_amplifier final : public engine::component
{
public:
    split_amplifier(std::size_t num_channels, std::string_view name)
        : m_gain_procs{algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [=](auto ch) {
                      return engine::make_event_to_audio_smoother_processor(
                              engine::default_smooth_length,
                              format_name(name, "gain", ch, num_channels));
                  })}
        , m_amp_procs{algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [=](auto ch) {
                      return engine::make_multiply_processor(
                              2,
                              format_name(name, "amp", ch, num_channels));
                  })}
        , m_inputs{algorithm::transform_to_vector(
                  m_amp_procs | boost::adaptors::indirected,
                  engine::make_graph_endpoint<0>)}
        , m_outputs{m_inputs}
        , m_event_inputs{algorithm::transform_to_vector(
                  m_gain_procs | boost::adaptors::indirected,
                  engine::make_graph_endpoint<0>)}
    {
    }

    [[nodiscard]] auto inputs() const -> endpoints override
    {
        return m_inputs;
    }

    [[nodiscard]] auto outputs() const -> endpoints override
    {
        return m_outputs;
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
        for (auto const& [gain_proc, amp_proc] :
             range::zip(m_gain_procs, m_amp_procs))
        {
            g.audio.insert({*gain_proc, 0}, {*amp_proc, 1});
        }
    }

private:
    std::vector<std::unique_ptr<engine::processor>> m_gain_procs;
    std::vector<std::unique_ptr<engine::processor>> m_amp_procs;

    std::vector<engine::graph_endpoint> m_inputs;
    std::vector<engine::graph_endpoint> m_outputs;
    std::vector<engine::graph_endpoint> m_event_inputs;
};

} // namespace

auto
make_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<amplifier>(num_channels, name);
}

auto
make_split_amplifier(std::size_t num_channels, std::string_view name)
        -> std::unique_ptr<engine::component>
{
    return std::make_unique<split_amplifier>(num_channels, name);
}

} // namespace piejam::audio::components
