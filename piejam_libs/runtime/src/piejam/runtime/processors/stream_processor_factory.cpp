// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/processors/stream_processor_factory.h>

#include <piejam/audio/engine/stream_processor.h>

namespace piejam::runtime::processors
{

stream_processor_factory::~stream_processor_factory() = default;

auto
stream_processor_factory::make_processor(
        audio_stream_id const id,
        std::size_t const capacity,
        std::string_view const& name) -> std::shared_ptr<processor_t>
{
    auto proc =
            std::make_shared<audio::engine::stream_processor>(capacity, name);
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
stream_processor_factory::takeover(stream_processor_factory& other)
{
    for (auto [id, proc] : other.m_procs)
    {
        if (!proc.expired())
            BOOST_VERIFY(m_procs.emplace(id, proc).second);
    }
}

} // namespace piejam::runtime::processors