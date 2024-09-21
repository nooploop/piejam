// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/audio/engine/stream_processor.h>

#include <boost/assert.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::runtime::processors
{

stream_processor_factory::~stream_processor_factory() = default;

auto
stream_processor_factory::make_processor(
        audio_stream_id const id,
        std::size_t const num_channels,
        std::size_t const capacity_per_channel,
        std::string_view const name) -> std::shared_ptr<processor_t>
{
    auto proc = std::make_shared<audio::engine::stream_processor>(
            num_channels,
            capacity_per_channel,
            name);
    BOOST_VERIFY(m_procs.emplace(id, proc).second);
    return proc;
}

auto
stream_processor_factory::find_processor(audio_stream_id const id) const
        -> std::shared_ptr<processor_t>
{
    auto it = m_procs.find(id);
    return it != m_procs.end() ? it->second.lock() : nullptr;
}

void
stream_processor_factory::clear_expired()
{
    std::erase_if(m_procs, boost::hof::unpack([](auto, auto const proc) {
                      return proc.expired();
                  }));
}

} // namespace piejam::runtime::processors
