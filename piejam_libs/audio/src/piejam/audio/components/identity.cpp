// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/components/identity.h>

#include <piejam/audio/engine/component.h>
#include <piejam/audio/engine/graph_endpoint.h>
#include <piejam/audio/engine/identity_processor.h>
#include <piejam/audio/engine/processor.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/range/indices.h>
#include <piejam/range/iota.h>

#include <vector>

namespace piejam::audio::components
{

namespace
{

class identity final : public engine::component
{
public:
    explicit identity(std::size_t num_channels)
        : m_procs{algorithm::transform_to_vector(
                  range::iota(num_channels),
                  [](auto) { return engine::make_identity_processor(); })}
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
        return {};
    }

    [[nodiscard]] auto event_outputs() const -> endpoints override
    {
        return {};
    }

    void connect(engine::graph&) const override
    {
    }

private:
    std::vector<std::unique_ptr<engine::processor>> m_procs;

    std::vector<engine::graph_endpoint> m_inputs{algorithm::transform_to_vector(
            range::indices(m_procs),
            [this](auto i) {
                return engine::graph_endpoint{.proc = *m_procs[i], .port = 0};
            })};
    std::vector<engine::graph_endpoint> m_outputs{m_inputs};
};

} // namespace

auto
make_identity(std::size_t num_channels) -> std::unique_ptr<engine::component>
{
    return std::make_unique<identity>(num_channels);
}

} // namespace piejam::audio::components
