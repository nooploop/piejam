// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::audio
{

enum class pcm_format : unsigned;

class device;
class device_manager;
class process_thread;

struct pcm_descriptor;
struct pcm_device_config;
struct pcm_hw_params;
struct pcm_process_config;
struct pcm_io_config;

template <class T>
struct pair;

class period_count;
class period_size;
class sample_rate;

enum class multichannel_layout : bool;
template <class T, class Layout, std::size_t NumChannels>
class multichannel_view;
template <class T, class Layout>
class multichannel_buffer;

} // namespace piejam::audio
