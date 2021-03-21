// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/fwd.h>
#include <piejam/audio/types.h>

#include <memory>

namespace piejam::audio
{

class device_manager
{
public:
    virtual ~device_manager() = default;

    virtual auto io_descriptors() -> pcm_io_descriptors = 0;
    virtual auto
    hw_params(pcm_descriptor const&, samplerate_t const*, period_size_t const*)
            -> pcm_hw_params = 0;

    virtual auto make_device(
            pcm_descriptor const& in,
            pcm_descriptor const& out,
            pcm_io_config const&) -> std::unique_ptr<device> = 0;
};

auto make_device_manager() -> std::unique_ptr<device_manager>;

} // namespace piejam::audio
