// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/event_port.h>
#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/engine/named_processor.h>

#include <boost/assert.hpp>

#include <array>
#include <memory>

namespace piejam::audio::engine
{

class event_identity_processor final : public named_processor
{
public:
    template <class T>
    event_identity_processor(std::in_place_type_t<T> t)
        : m_event_inputs({event_port(t, "in")})
        , m_event_outputs({event_port(t, "out")})
    {
    }

    [[nodiscard]] auto type_name() const noexcept -> std::string_view override
    {
        return "event_identity";
    }

    [[nodiscard]] auto num_inputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]] auto num_outputs() const noexcept -> std::size_t override
    {
        return 0;
    }

    [[nodiscard]] auto event_inputs() const noexcept -> event_ports override
    {
        return m_event_inputs;
    }

    [[nodiscard]] auto event_outputs() const noexcept -> event_ports override
    {
        return m_event_outputs;
    }

    void process(process_context const&) override
    {
        BOOST_ASSERT_MSG(
                false,
                "Should be removed from graph before executing.");
    }

private:
    std::array<event_port, 1> m_event_inputs;
    std::array<event_port, 1> m_event_outputs;
};

template <class T>
auto
make_event_identity_processor() -> std::unique_ptr<processor>
{
    return std::make_unique<event_identity_processor>(std::in_place_type<T>);
}

inline auto
is_event_identity_processor(processor const& p) noexcept -> bool
{
    return typeid(p) == typeid(event_identity_processor);
}

} // namespace piejam::audio::engine
