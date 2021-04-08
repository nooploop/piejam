// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/components/fx_scope.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/stream_processor.h>
#include <piejam/runtime/fx/module.h>
#include <piejam/runtime/fx/parameter.h>
#include <piejam/runtime/fx/scope.h>
#include <piejam/runtime/parameter_processor_factory.h>
#include <piejam/runtime/processors/stream_processor_factory.h>

#include <fmt/format.h>

#include <boost/container/flat_map.hpp>

#include <array>

namespace piejam::runtime::components
{

namespace
{

constexpr auto
stream_capacity(audio::samplerate_t const samplerate) -> std::size_t
{
    return static_cast<std::size_t>((samplerate / 1000.f) * 17 * 3);
}

class fx_scope final : public audio::engine::component
{
public:
    fx_scope(
            fx::module const& fx_mod,
            audio::samplerate_t const samplerate,
            processors::stream_processor_factory& stream_proc_factory,
            std::string_view const&)
        : m_stream_proc(stream_proc_factory.make_processor(
                  fx_mod.streams->at(static_cast<fx::stream_key>(
                          fx::scope_stream_key::left_right)),
                  2,
                  stream_capacity(samplerate),
                  "scope"))
    {
    }

    auto inputs() const -> endpoints override { return m_inputs; }
    auto outputs() const -> endpoints override { return m_outputs; }

    auto event_inputs() const -> endpoints override { return {}; }
    auto event_outputs() const -> endpoints override { return {}; }

    void connect(audio::engine::graph& g) const override
    {
        g.add_wire(
                {.proc = *m_left_identity, .port = 0},
                {.proc = *m_stream_proc, .port = 0});
        g.add_wire(
                {.proc = *m_right_identity, .port = 0},
                {.proc = *m_stream_proc, .port = 1});
    }

private:
    std::unique_ptr<audio::engine::processor> m_left_identity{
            audio::engine::make_identity_processor()};
    std::unique_ptr<audio::engine::processor> m_right_identity{
            audio::engine::make_identity_processor()};
    std::shared_ptr<audio::engine::processor> m_stream_proc;

    std::array<audio::engine::graph_endpoint, 2> m_inputs{
            audio::engine::graph_endpoint{*m_left_identity, 0},
            audio::engine::graph_endpoint{*m_right_identity, 0}};
    std::array<audio::engine::graph_endpoint, 2> m_outputs{
            audio::engine::graph_endpoint{*m_left_identity, 0},
            audio::engine::graph_endpoint{*m_right_identity, 0}};
};

} // namespace

auto
make_fx_scope(
        fx::module const& fx_mod,
        audio::samplerate_t const samplerate,
        processors::stream_processor_factory& stream_proc_factory,
        std::string_view const& name)
        -> std::unique_ptr<audio::engine::component>
{
    return std::make_unique<fx_scope>(
            fx_mod,
            samplerate,
            stream_proc_factory,
            name);
}

} // namespace piejam::runtime::components
