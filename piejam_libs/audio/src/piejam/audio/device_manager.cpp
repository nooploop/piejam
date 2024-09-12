// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/audio/device_manager.h>

#include "alsa/get_pcm_io_descriptors.h"
#include "alsa/get_set_hw_params.h"
#include "alsa/pcm_io.h"

#include <piejam/audio/pcm_descriptor.h>
#include <piejam/audio/pcm_hw_params.h>

#include <piejam/box.h>
#include <piejam/io_pair.h>

namespace piejam::audio
{

namespace
{

class alsa_device_manager final : public device_manager
{
public:
    auto io_descriptors() -> pcm_io_descriptors override
    {
        return alsa::get_pcm_io_descriptors();
    }

    auto hw_params(
            pcm_descriptor const& d,
            sample_rate const* const sample_rate,
            period_size const* const period_size) -> pcm_hw_params override
    {
        return alsa::get_hw_params(d, sample_rate, period_size);
    }

    auto make_device(
            pcm_descriptor const& in,
            pcm_descriptor const& out,
            pcm_io_config const& config) -> std::unique_ptr<device> override
    {
        return std::make_unique<alsa::pcm_io>(in, out, config);
    }
};

} // namespace

auto
make_device_manager() -> std::unique_ptr<device_manager>
{
    return std::make_unique<alsa_device_manager>();
}

} // namespace piejam::audio
