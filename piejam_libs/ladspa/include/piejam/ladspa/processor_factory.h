// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/audio/engine/fwd.h>
#include <piejam/audio/fwd.h>
#include <piejam/ladspa/fwd.h>

#include <memory>

namespace piejam::ladspa
{

class processor_factory
{
public:
    virtual ~processor_factory() = default;

    virtual auto make_processor(instance_id const&, audio::sample_rate)
            -> std::unique_ptr<audio::engine::processor> = 0;
};

} // namespace piejam::ladspa
