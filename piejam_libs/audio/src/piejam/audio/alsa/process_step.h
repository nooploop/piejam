// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/pcm_io_config.h>
#include <piejam/audio/process_function.h>
#include <piejam/numeric/rolling_mean.h>
#include <piejam/system/fwd.h>

#include <atomic>
#include <memory>
#include <system_error>
#include <vector>

namespace piejam::audio::alsa
{

class pcm_reader;
class pcm_writer;

class process_step
{
public:
    process_step(
            system::device& input_fd,
            system::device& output_fd,
            pcm_io_config const&,
            std::atomic<float>& cpu_load,
            std::atomic_size_t& xruns,
            init_process_function const&,
            process_function);
    process_step(process_step&&);
    ~process_step();

    [[nodiscard]] auto operator()() -> std::error_condition;

private:
    system::device& m_input_fd;
    system::device& m_output_fd;
    pcm_io_config m_io_config;
    std::atomic<float>& m_cpu_load;
    std::atomic_size_t& m_xruns;
    process_function m_process_function;

    bool m_starting{true};
    std::unique_ptr<pcm_reader> m_reader;
    std::unique_ptr<pcm_writer> m_writer;
    numeric::rolling_mean<float> m_cpu_load_mean_acc;
};

} // namespace piejam::audio::alsa
