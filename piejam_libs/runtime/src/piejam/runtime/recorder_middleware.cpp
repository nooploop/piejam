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

    next(a);

    auto const& recorder_streams = *get_state().recorder_streams;
    if (recorder_streams.empty())
        return;

    auto take_dir = m_impl->recordings_dir /
                    fmt::format("session_{:04}", st.rec_session) /
                    fmt::format("take_{:04}", st.rec_take);

    if (!std::filesystem::create_directories(take_dir))
    {
        spdlog::error(
                "Could not create recordings directory: {}",
                std::strerror(errno));
        return;
    }

    impl::open_streams_t open_streams;
    open_streams.reserve(recorder_streams.size());

    for (auto const& [mixer_channel_id, stream_id] : recorder_streams)
    {
        mixer::channel const* const mixer_channel =
                get_state().mixer_state.channels.find(mixer_channel_id);
        BOOST_ASSERT(mixer_channel);

        auto filename = take_dir / fmt::format("{}.wav", *mixer_channel->name);

        std::size_t file_num{};
        while (std::filesystem::exists(filename))
            filename = take_dir / fmt::format(
                                          "{} ({}).wav",
                                          *mixer_channel->name,
                                          ++file_num);

        auto const format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
        if (SndfileHandle::formatCheck(format, 2, st.sample_rate.as_int()))
        {
            SndfileHandle sndfile(
                    filename.string(),
                    SFM_WRITE,
                    format,
                    2,
                    st.sample_rate.as_int());

            if (sndfile)
                open_streams.emplace(stream_id, std::move(sndfile));
            else
                spdlog::error(
                        "Could not create file for recording: {}",
                        sndfile.strError());
        }
        else
        {
            spdlog::error(
                    "Could not create file for recording: Invalid file "
                    "format.");
        }
    }

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
            auto const num_frames = buffer->num_frames();
            auto const written =
                    it->second.writef(buffer->samples().data(), num_frames);
            if (static_cast<std::size_t>(written) < num_frames)
                spdlog::warn(
                        "Could not write {} frames: {}",
                        num_frames - written,
                        it->second.strError());
        }
    }

    next(a);
}

} // namespace piejam::runtime
