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
struct pcm_io_descriptors;
struct pcm_process_config;
struct pcm_io_config;

template <class T>
struct pair;

} // namespace piejam::audio
