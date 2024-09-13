// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstddef>

namespace piejam::audio
{

enum class pcm_format : unsigned;

class io_process;
class sound_card_manager;
class process_thread;

struct sound_card_descriptor;
struct pcm_device_config;
struct sound_card_hw_params;
struct pcm_process_config;
struct io_process_config;

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
