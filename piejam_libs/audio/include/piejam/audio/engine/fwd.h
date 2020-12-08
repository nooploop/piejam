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

namespace piejam::audio::engine
{

template <class T>
class event;
class abstract_event_buffer;
template <class T>
class event_buffer;
class event_buffer_memory;
class event_input_buffers;
class event_output_buffers;
class event_port;
template <class T>
class slice;
using audio_slice = slice<float>;

class component;
class processor;
class named_processor;
class input_processor;
class output_processor;
template <class T>
class value_input_processor;

class dag;
class dag_executor;
class graph;
struct graph_endpoint;
class process;
struct process_context;
class processor_job;
class thread_context;

} // namespace piejam::audio::engine
