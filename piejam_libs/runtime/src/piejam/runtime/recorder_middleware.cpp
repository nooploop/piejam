// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/recorder_middleware.h>

#include <piejam/runtime/actions/recorder_action.h>
#include <piejam/runtime/actions/recording.h>
#include <piejam/runtime/actions/request_streams_update.h>
#include <piejam/runtime/actions/update_streams.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/action.h>

#include <sndfile.hh>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>
#include <boost/container/flat_map.hpp>

namespace piejam::runtime
{

struct recorder_middleware::impl
{
    using open_streams_t =
            boost::container::flat_map<audio_stream_id, SndfileHandle>;

    std::filesystem::path recordings_dir;
    open_streams_t open_streams;
};

recorder_middleware::recorder_middleware(
        middleware_functors mw_fs,
        std::filesystem::path recordings_dir)
    : middleware_functors(std::move(mw_fs))
    , m_impl(std::make_unique<impl>(std::move(recordings_dir)))
{
}

recorder_middleware::~recorder_middleware() = default;

void
recorder_middleware::operator()(action const& act)
{
    if (auto const* a = dynamic_cast<actions::recorder_action const*>(&act))
    {
        auto v = ui::make_action_visitor<actions::recorder_action_visitor>(
                [this](auto const& a) { process_recorder_action(a); });
        a->visit(v);
    }
    else
    {
        next(act);
    }
}

void
recorder_middleware::process_recorder_action(actions::start_recording const& a)
{
    auto const& st = get_state();

    BOOST_ASSERT(!st.recording);

    auto take_dir = m_impl->recordings_dir /
                    fmt::format("session_{:04}", st.rec_session) /
                    fmt::format("take_{:04}", st.rec_take);

    if (!std::filesystem::create_directories(take_dir))
    {
        spdlog::error("Could not create recordings directory.");
        return;
    }

    next(a);

    auto const& recorder_streams = *get_state().recorder_streams;

    impl::open_streams_t open_streams;
    open_streams.reserve(recorder_streams.size());

    for (auto const& [channel_id, stream_id] : recorder_streams)
    {
        auto const* const channel =
                get_state().mixer_state.channels.find(channel_id);
        BOOST_ASSERT(channel);
        BOOST_ASSERT(channel->record);

        auto filename = take_dir / fmt::format("{}.flac", *channel->name);

        open_streams.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(stream_id),
                std::forward_as_tuple(
                        filename.string(),
                        SFM_WRITE,
                        SF_FORMAT_FLAC,
                        2,
                        static_cast<int>(st.sample_rate)));
    }

    BOOST_ASSERT(m_impl->open_streams.empty());
    m_impl->open_streams = std::move(open_streams);
}

void
recorder_middleware::process_recorder_action(actions::stop_recording const& a)
{
    m_impl->open_streams.clear();

    next(a);
}

void
recorder_middleware::process_recorder_action(actions::update_streams const& a)
{
    for (auto const& [stream_id, buffer] : a.streams)
    {
        if (auto it = m_impl->open_streams.find(stream_id);
            it != m_impl->open_streams.end())
        {
            it->second.write(
                    buffer->data().data(),
                    std::ranges::distance(*buffer));
        }
    }

    next(a);
}

} // namespace piejam::runtime
