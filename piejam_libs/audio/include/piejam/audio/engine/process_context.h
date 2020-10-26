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

#include <functional>
#include <span>

namespace piejam::audio::engine
{

class event_input_buffers;
class event_output_buffers;

struct process_context
{
    using input_buffers_t = std::span<
            std::reference_wrapper<std::span<float const> const> const>;
    using output_buffers_t = std::span<std::span<float> const>;
    using result_buffers_t = std::span<std::span<float const>>;

    input_buffers_t inputs{};
    output_buffers_t outputs{};
    result_buffers_t results{};
    event_input_buffers const& event_inputs;
    event_output_buffers const& event_outputs;
    std::size_t buffer_size{};
};

} // namespace piejam::audio::engine
