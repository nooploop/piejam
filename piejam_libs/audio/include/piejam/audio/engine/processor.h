// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>

#include <span>
#include <string_view>

namespace piejam::audio::engine
{

class processor
{
public:
    using event_ports = std::span<event_port const>;

    virtual ~processor() = default;

    [[nodiscard]]
    virtual auto type_name() const noexcept -> std::string_view = 0;
    [[nodiscard]]
    virtual auto name() const noexcept -> std::string_view = 0;

    [[nodiscard]]
    virtual auto num_inputs() const noexcept -> std::size_t = 0;
    [[nodiscard]]
    virtual auto num_outputs() const noexcept -> std::size_t = 0;

    [[nodiscard]]
    virtual auto event_inputs() const noexcept -> event_ports = 0;
    [[nodiscard]]
    virtual auto event_outputs() const noexcept -> event_ports = 0;

    virtual void process(process_context const&) = 0;
};

} // namespace piejam::audio::engine
