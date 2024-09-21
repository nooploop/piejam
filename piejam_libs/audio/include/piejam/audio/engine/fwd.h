// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

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
class stream_processor;
template <class T>
class value_io_processor;

class dag;
class dag_executor;
class graph;
struct graph_endpoint;
class process;
struct process_context;
class processor_job;
class thread_context;

} // namespace piejam::audio::engine
